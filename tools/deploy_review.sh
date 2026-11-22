#!/bin/sh
# make deploy-review: the browser version with the game and the HD art
# (make web GAME=1: it distributes the game's files, so for a private site
# only: docs/BROWSER.md)
# served by Caddy in Docker (container
# civ2-review, restarted with the machine) at https://HOST/civ2/, behind a
# user name and password (HTTP Basic Auth), the root and everything else a
# plain 404.  HOST is this machine's address as an sslip.io name
# (139-162-93-75.sslip.io), which Caddy gets a Let's Encrypt certificate
# for (ports 80 and 443; 80 redirects; the certificate kept in the Docker
# volume civ2-review-data).  REVIEW_HTTP=1: plain http on port 80 instead
# (the password then goes in clear text), kept for later runs until
# REVIEW_HTTP= (empty) asks for https again.
#
# Other projects add their routes as $HOME/web-routes/*.caddy (imported into
# the site, beside /civ2/, without its password).
#
# Outside the repository, in $REVIEW_DIR (~/civ2-review): site/ (a copy of
# build/web under civ2/), Caddyfile, and config (mode 600: REVIEW_USER,
# REVIEW_PASSWORD, REVIEW_HASH), made on the first run with a random
# password and kept after; REVIEW_NEW_PASSWORD=1 makes a new one, and
# REVIEW_USER=NAME REVIEW_PASSWORD=WORD set them.
#
#   tools/deploy_review.sh            build, copy, (re)start, print the addresses
#   docker rm -f civ2-review          take it down
set -e
cd "$(dirname "$0")/.."
DIR=${REVIEW_DIR:-$HOME/civ2-review}
NAME=civ2-review
IMAGE=caddy:2.8.4-alpine
IP=${REVIEW_IP:-$(hostname -I | awk '{print $1}')}
HOST=${REVIEW_HOST:-$(echo "$IP" | tr . -).sslip.io}

mkdir -p "$DIR/site/civ2"
CONFIG=$DIR/config
if [ ! -f "$CONFIG" ] || [ -n "$REVIEW_NEW_PASSWORD" ] || [ -n "$REVIEW_PASSWORD" ]; then
	PASSWORD=${REVIEW_PASSWORD:-$(python3 -c "import secrets, string; a = string.ascii_letters + string.digits; print(''.join(secrets.choice(a) for _ in range(16)))")}
	HASH=$(docker run --rm "$IMAGE" caddy hash-password --plaintext "$PASSWORD")
	umask 077
	printf "REVIEW_USER='%s'\nREVIEW_PASSWORD='%s'\nREVIEW_HASH='%s'\n" "${REVIEW_USER:-reviewer}" "$PASSWORD" "$HASH" > "$CONFIG"
	chmod 600 "$CONFIG"
	umask 022
fi
# (REVIEW_HTTP=1 or REVIEW_HTTP= given: kept in the config for the next runs)
HTTP_ASKED=${REVIEW_HTTP+set} HTTP_GIVEN=$REVIEW_HTTP
. "$CONFIG"
if [ -n "$HTTP_ASKED" ]; then
	REVIEW_HTTP=$HTTP_GIVEN
	grep -v '^REVIEW_HTTP=' "$CONFIG" > "$CONFIG.new" || true
	echo "REVIEW_HTTP='$REVIEW_HTTP'" >> "$CONFIG.new"
	chmod 600 "$CONFIG.new"
	mv "$CONFIG.new" "$CONFIG"
fi

# (with the HD art, so that HD changes can be checked there: make web runs
# make hd-art's tools/build_hd_pack.py, which copies it from the cache,
# ~/.cache/openciv2, without the network once made)
make web GAME=1
rsync -a --delete build/web/ "$DIR/site/civ2/"

if [ -n "$REVIEW_HTTP" ]; then
	ADDRESS=":80"
	URL="http://$IP"
	PORTS="-p 80:80"
else
	ADDRESS="$HOST"
	URL="https://$HOST"
	PORTS="-p 80:80 -p 443:443 -p 443:443/udp"
fi

# (the hash's $ are not the shell's: a quoted here-document, then filled in)
cat > "$DIR/Caddyfile" <<'EOF'
# tools/deploy_review.sh writes this: the site at /civ2/, behind a password, nothing else
{
	admin off
}
@ADDRESS@ {
	header X-Robots-Tag "noindex, nofollow"
	handle /robots.txt {
		respond <<TXT
			User-agent: *
			Disallow: /
			TXT 200
	}
	redir /civ2 /civ2/ 308
	handle /civ2/* {
		basic_auth {
			@USER@ @HASH@
		}
		root * /srv
		# (text and the modules compressed; the game's files, the art and
		# the .gz files as they are: the page reads their lengths, inflates
		# the .gz itself)
		encode {
			zstd
			gzip
			match {
				header Content-Type text/*
				header Content-Type application/javascript*
				header Content-Type application/json*
				header Content-Type application/wasm*
			}
		}
		# (the big files that do not change: the game's videos and music,
		# read when the game opens them; private: behind the password)
		@media path /civ2/game/files/*
		header @media Cache-Control "private, max-age=604800"
		@other not path /civ2/game/files/*
		header @other Cache-Control "private, no-cache"
		@pack path *.pack
		header @pack Content-Type application/octet-stream
		file_server
	}
	# (other projects' routes on this server: $HOME/web-routes/*.caddy, none or
	# more; outside /civ2/, so not behind its password)
	import /etc/caddy/routes/*.caddy
	handle {
		respond 404
	}
}
EOF
python3 - "$DIR/Caddyfile" "$ADDRESS" "$REVIEW_USER" "$REVIEW_HASH" <<'EOF'
import sys
p, address, user, hash_ = sys.argv[1:]
s = open(p).read().replace('@ADDRESS@', address).replace('@USER@', user).replace('@HASH@', hash_)
open(p, 'w').write(s)
EOF

mkdir -p "$HOME/web-routes"
docker rm -f "$NAME" >/dev/null 2>&1 || true
# shellcheck disable=SC2086
docker run -d --name "$NAME" --restart unless-stopped $PORTS -v civ2-review-data:/data \
	-v "$HOME/web-routes":/etc/caddy/routes:ro -v "$DIR/site":/srv:ro -v "$DIR/Caddyfile":/etc/caddy/Caddyfile:ro "$IMAGE" >/dev/null

echo "OpenCiv2 for reviewers (container $NAME; down: docker rm -f $NAME)"
echo "  user $REVIEW_USER, password in $CONFIG"
echo "  $URL/civ2/                English"
for code in $(python3 -c "import json; print(' '.join(l['code'] for l in json.load(open('build/web/lang/index.json'))))"); do
	echo "  $URL/civ2/?lang=$code"
done
