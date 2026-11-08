/*
 * Fonts and text, from the glyphs the reference environment draws: Times
 * New Roman, Arial, System and Courier at 6..48 pixels, regular/bold/italic.
 * Wine draws 1-bit glyphs on surfaces of 8 bits or less
 * (data/native/fonts.bin, tools/probe/fontdump.c) and antialiased ones on
 * deeper surfaces (data/native/fonts_aa.bin, tools/probe/fontdump_aa.c),
 * blended as its DIB engine does (tools/probe/aaprobe.c checks the model).
 */
#include "plat.h"
#include "hd.h"
#include "../host/host.h"
#include <math.h>

DC *dc_get(HDC h);

typedef struct {
    short adv;                      /* for layout (GetTextExtentPoint) */
    short dadv;                     /* for drawing: Wine's may differ by a pixel */
    signed char x, y;
    unsigned char w, h;
    const unsigned char *bits;      /* (w+7)/8 bytes per row */
} Glyph;

typedef struct {
    signed char x, y;
    unsigned char w, h;
    unsigned short n;
    const unsigned char *rle;       /* run-length coded coverage 0..16 */
    unsigned char *cov;             /* decoded on first use */
} AaGlyph;

struct FontFace {
    int face, height, bold, italic;
    int tm[11];
    Glyph g[256];
    AaGlyph *aa;                    /* 256 entries, loaded on demand */
};

#define NFACES 4
#define MINH 6
#define MAXH 48
static FontFace *faces[NFACES][MAXH + 1][2][2];
static unsigned char *font_data, *aa_data;
extern char plat_data_dir[];

static void load_aa(void)
{
    char path[1024];
    FILE *f;
    long n;
    unsigned char *p, *end;
    unsigned count, i;
    snprintf(path, sizeof path, "%s/fonts_aa.bin", plat_data_dir);
    f = fopen(path, "rb");
    if (!f)
        plat_fatal("cannot open %s", path);
    fseek(f, 0, SEEK_END);
    n = ftell(f);
    fseek(f, 0, SEEK_SET);
    aa_data = malloc((size_t)n);
    if (fread(aa_data, 1, (size_t)n, f) != (size_t)n || memcmp(aa_data, "C2FONTAA", 8))
        plat_fatal("%s: not an antialiased font file", path);
    fclose(f);
    p = aa_data + 12;
    end = aa_data + n;
    memcpy(&count, aa_data + 8, 4);
    for (i = 0; i < count && p < end; i++) {
        int face = p[0], height = p[1], bold = (p[2] | p[3] << 8) >= 600, it = p[4] != 0, c;
        FontFace *ff = face < NFACES && height >= MINH && height <= MAXH ? faces[face][height][bold][it] : NULL;
        AaGlyph *g = calloc(256, sizeof *g);
        p += 8 + 44;
        for (c = 0; c < 256; c++) {
            g[c].x = (signed char)p[2];
            g[c].y = (signed char)p[3];
            g[c].w = p[4];
            g[c].h = p[5];
            g[c].n = (unsigned short)(p[6] | p[7] << 8);
            g[c].rle = p + 8;
            p += 8 + g[c].n;
        }
        if (ff)
            ff->aa = g;
        else
            free(g);
    }
}

static const unsigned char *aa_coverage(AaGlyph *g)
{
    if (!g->cov && g->w && g->h) {
        const unsigned char *p = g->rle, *end = g->rle + g->n;
        int o = 0, total = g->w * g->h;
        g->cov = calloc((size_t)total, 1);
        while (p < end && o < total) {
            int b = *p++;
            if (b < 0x80) {
                int k;
                for (k = 0; k <= b && p < end && o < total; k++)
                    g->cov[o++] = *p++;
            } else {
                int k, v = p < end ? *p++ : 0;
                for (k = 0; k < b - 0x80 + 2 && o < total; k++)
                    g->cov[o++] = (unsigned char)v;
            }
        }
    }
    return g->cov;
}

/* Wine's grayscale text blend (dibdrv): a gamma ramp of 17 coverage levels. */
static const unsigned char aa_ramp[17] = {0, 0x4d, 0x68, 0x7c, 0x8c, 0x9a, 0xa7, 0xb2, 0xbd, 0xc7, 0xd0,
                                          0xd9, 0xe1, 0xe9, 0xf0, 0xf8, 0xff};

static unsigned aa_blend(unsigned dst, unsigned text, int aa)
{
    unsigned mn, mx;
    if (aa >= 16)
        return text;
    if (aa <= 1 || dst == text)     /* the lowest level leaves the pixel unchanged */
        return dst;
    mn = (aa_ramp[aa] * text) / 0xff;
    mx = aa_ramp[16 - aa] + ((0xff - aa_ramp[16 - aa]) * text) / 0xff;
    if (dst > text)
        return text + ((dst - text) * (mx - text)) / (0xff - text);
    return text - ((text - dst) * (text - mn)) / text;
}

static void load_fonts(void)
{
    char path[1024];
    FILE *f;
    long n;
    unsigned char *p, *end;
    unsigned count, i;
    snprintf(path, sizeof path, "%s/fonts.bin", plat_data_dir);
    f = fopen(path, "rb");
    if (!f)
        plat_fatal("cannot open %s", path);
    fseek(f, 0, SEEK_END);
    n = ftell(f);
    fseek(f, 0, SEEK_SET);
    font_data = malloc((size_t)n);
    if (fread(font_data, 1, (size_t)n, f) != (size_t)n)
        plat_fatal("cannot read %s", path);
    fclose(f);
    if (memcmp(font_data, "C2FONTS2", 8))
        plat_fatal("%s: not a font file", path);
    p = font_data + 8;
    end = font_data + n;
    memcpy(&count, p, 4);
    p += 4;
    for (i = 0; i < count && p < end; i++) {
        FontFace *ff = calloc(1, sizeof *ff);
        int c, k;
        ff->face = p[0];
        ff->height = p[1];
        ff->bold = (p[2] | p[3] << 8) >= 600;
        ff->italic = p[4] != 0;
        p += 8;
        for (k = 0; k < 11; k++, p += 4)
            memcpy(&ff->tm[k], p, 4);
        for (c = 0; c < 256; c++) {
            Glyph *g = &ff->g[c];
            memcpy(&g->adv, p, 2);
            memcpy(&g->dadv, p + 2, 2);
            g->x = (signed char)p[4];
            g->y = (signed char)p[5];
            g->w = p[6];
            g->h = p[7];
            p += 8;
            g->bits = p;
            p += (size_t)((g->w + 7) / 8) * g->h;
        }
        if (ff->face < NFACES && ff->height >= MINH && ff->height <= MAXH)
            faces[ff->face][ff->height][ff->bold][ff->italic] = ff;
    }
}

/* Wine's substitutes: Times New Roman -> DejaVu Serif, Arial and System ->
 * DejaVu Sans, Courier -> DejaVu Sans Mono. */
static int face_index(const LOGFONTA *lf)
{
    const char *n = lf->lfFaceName;
    if (!strcasecmp(n, "Times New Roman") || !strncasecmp(n, "DejaVu Serif", 12) || !strcasecmp(n, "Times"))
        return 0;
    if (!strcasecmp(n, "Arial") || !strcasecmp(n, "Tahoma") || !strcasecmp(n, "MS Sans Serif") ||
        !strcasecmp(n, "MS Shell Dlg") ||
        !strcasecmp(n, "DejaVu Sans"))
        return 1;
    if (!strcasecmp(n, "System"))
        return 2;
    if (!strcasecmp(n, "Courier") || !strcasecmp(n, "Courier New") || !strncasecmp(n, "DejaVu Sans Mono", 16))
        return 3;
    switch (lf->lfPitchAndFamily & 0xf0) {
    case FF_ROMAN: return 0;
    case FF_MODERN: return 3;
    }
    return 1;
}

FontFace *font_face_for(const LOGFONTA *lf)
{
    int face, h, bold = lf->lfWeight >= 600, it = lf->lfItalic != 0;
    if (!font_data)
        load_fonts();
    face = face_index(lf);
    if (lf->lfHeight < 0) {
        h = -lf->lfHeight;
    } else if (lf->lfHeight == 0) {
        h = 16;
    } else {
        /* A cell height: the largest font whose cell fits. */
        int k;
        h = MINH;
        for (k = MINH; k <= MAXH; k++)
            if (faces[face][k][bold][it] && faces[face][k][bold][it]->tm[0] <= lf->lfHeight)
                h = k;
    }
    if (h < MINH) h = MINH;
    if (h > MAXH) h = MAXH;
    return faces[face][h][bold][it];
}

void text_metrics(FontFace *f, TEXTMETRICA *tm)
{
    memset(tm, 0, sizeof *tm);
    tm->tmHeight = f->tm[0];
    tm->tmAscent = f->tm[1];
    tm->tmDescent = f->tm[2];
    tm->tmInternalLeading = f->tm[3];
    tm->tmExternalLeading = f->tm[4];
    tm->tmAveCharWidth = f->tm[5];
    tm->tmMaxCharWidth = f->tm[6];
    tm->tmWeight = f->tm[7];
    tm->tmOverhang = f->tm[8];
    tm->tmFirstChar = (BYTE)f->tm[9];
    tm->tmLastChar = (BYTE)(f->tm[9] >> 8);
    tm->tmDefaultChar = (BYTE)(f->tm[9] >> 16);
    tm->tmBreakChar = (BYTE)(f->tm[9] >> 24);
    tm->tmPitchAndFamily = (BYTE)f->tm[10];
    tm->tmCharSet = (BYTE)(f->tm[10] >> 8);
    tm->tmItalic = (BYTE)(f->italic ? 255 : 0);
    tm->tmDigitizedAspectX = tm->tmDigitizedAspectY = 96;
}

/* (port) Text is measured as it is drawn (dadv): Wine measures a bold line
   a few pixels narrower than it draws it, so the game's word wrapping ran
   lines past their boxes (the heralds question).  CIV2_WINE_TEXT=1 keeps
   Wine's measures, for comparing with the original's recordings
   (tools/native-run.sh). */
int text_width(FontFace *f, const char *s, int n)
{
    static int wine = -1;
    int i, w = 0;
    if (wine < 0)
        wine = getenv("CIV2_WINE_TEXT") != NULL;
    for (i = 0; i < n; i++)
        w += wine ? f->g[(unsigned char)s[i]].adv : f->g[(unsigned char)s[i]].dadv;
    return w;
}

int text_height(FontFace *f) { return f->tm[0]; }

static FontFace *dc_face(DC *dc)
{
    Font *font = obj_get(dc->hfont, OBJ_FONT);
    return font ? font->face : NULL;
}

/* Glyphs at logical (x, y) = top left of the text cell, clipped to `clip`
 * (logical) and the DC's clip region. */
void text_draw(DC *dc, int x, int y, const char *s, int n, const RECT *clip)
{
    FontFace *f = dc_face(dc);
    Bitmap *b;
    Region rg = {0, 0, NULL};
    COLORREF c;
    int i, ox, oy;
    if (!f || n <= 0)
        return;
    b = dc_target(dc);
    if (hd_enabled && !b->hd) hd_sync(b);
    if (dc->is_memory) {
        RECT all = {0, 0, 0, 0};
        all.right = b->w;
        all.bottom = b->h;
        rgn_set(&rg, &all);
    } else {
        rgn_copy(&rg, &dc->clip);
    }
    ox = dc->origin.x;
    oy = dc->origin.y;
    if (clip) {
        RECT dclip = *clip;
        dclip.left += ox; dclip.right += ox; dclip.top += oy; dclip.bottom += oy;
        rgn_intersect_rect(&rg, &dclip);
    }
    if (dc->bk_mode == OPAQUE) {
        RECT bg, d;
        int k;
        bg.left = x + ox; bg.top = y + oy;
        bg.right = bg.left + text_width(f, s, n);
        bg.bottom = bg.top + f->tm[0];
        for (k = 0; k < rg.n; k++)
            if (rect_intersect(&d, &rg.r[k], &bg)) {
                int px, py;
                COLORREF bk = dc_resolve_color(dc, dc->bk_color);
                for (py = d.top; py < d.bottom; py++)
                    for (px = d.left; px < d.right; px++)
                        surf_put(b, px, py, bk);
            }
    }
    c = dc_resolve_color(dc, dc->text_color);
    if (hd_enabled) {
        int d=hd_density, fh=f->height*d;
        int cpu_text=0,read_l=b->w*d,read_t=b->h*d,read_r=0,read_b=0;
        FontFace *fine;
        if(fh>MAXH)fh=MAXH;
        fine=faces[f->face][fh][f->bold][f->italic];
        if(!aa_data)load_aa();
        if(!fine || !fine->aa)fine=f;
        cpu_text=b->hd_gpu && !(host_gpu_available()&2) && c!=RGB(0,0,0) && c!=RGB(255,255,255);
        hd_gpu_text_begin();
        if(cpu_text) {
            int advance=x,k;
            /* One readback/upload per string, not per glyph. The exact colored
               text gamma ramp depends on destination RGB. */
            for(i=0;i<n && fine->aa;i++) {
                unsigned ch=(unsigned char)s[i];
                AaGlyph *g=&fine->aa[ch];
                double scale=(double)f->height*d/fine->height;
                double xs=fine->g[ch].adv ? (double)f->g[ch].dadv*d/fine->g[ch].adv : scale;
                int gx=(advance+ox)*d+(int)(g->x*xs),gy=(y+oy)*d+(int)(g->y*scale);
                int gw=(int)(g->w*xs+.5),gh=(int)(g->h*scale+.5);
                for(k=0;k<rg.n;k++) {
                    int l=rg.r[k].left*d,t=rg.r[k].top*d,r=rg.r[k].right*d,bot=rg.r[k].bottom*d;
                    if(l<gx)l=gx;if(t<gy)t=gy;if(r>gx+gw)r=gx+gw;if(bot>gy+gh)bot=gy+gh;
                    if(l<0)l=0;if(t<0)t=0;if(r>b->w*d)r=b->w*d;if(bot>b->h*d)bot=b->h*d;
                    if(l<r && t<bot) { if(l<read_l)read_l=l;if(t<read_t)read_t=t;if(r>read_r)read_r=r;if(bot>read_b)read_b=bot; }
                }
                advance+=f->g[ch].dadv;
            }
            hd_read_region(b,read_l,read_t,read_r-read_l,read_b-read_t);
        }
        for(i=0;i<n && fine->aa;i++) {
            unsigned ch=(unsigned char)s[i];
            AaGlyph *g=&fine->aa[ch];
            const unsigned char *cov=aa_coverage(g);
            double scale=(double)f->height*d/fine->height;
            /* Hinting at the larger size must not widen a glyph beyond the
               logical advance used by the original layout and hit testing. */
            double xs=fine->g[ch].adv ? (double)f->g[ch].dadv*d/fine->g[ch].adv : scale;
            int gx=(x+ox)*d+(int)(g->x*xs),gy=(y+oy)*d+(int)(g->y*scale);
            int gw=(int)(g->w*xs+.5),gh=(int)(g->h*scale+.5),k,row,col;
            for(k=0;cov && k<rg.n;k++) {
                int l=rg.r[k].left*d,t=rg.r[k].top*d,r=rg.r[k].right*d,bot=rg.r[k].bottom*d;
                if(l<gx)l=gx;if(t<gy)t=gy;if(r>gx+gw)r=gx+gw;if(bot>gy+gh)bot=gy+gh;
                if(l<0)l=0;if(t<0)t=0;if(r>b->w*d)r=b->w*d;if(bot>b->h*d)bot=b->h*d;
                if(!cpu_text && hd_gpu_text(b,cov,g->w,g->h,gx,gy,gw,gh,l,t,r,bot,
                               GetRValue(c)<<16|GetGValue(c)<<8|GetBValue(c)))continue;
                if(!cpu_text)hd_read_region(b,l,t,r-l,bot-t);
                for(row=t;row<bot;row++)for(col=l;col<r;col++) {
                    int v=cov[((row-gy)*g->h/gh)*g->w+(col-gx)*g->w/gw];
                    uint32_t *dst=b->hd+row*b->w*d+col,z=*dst;
                    *dst=0xff000000u|aa_blend((z>>16)&255,GetRValue(c),v)<<16|
                         aa_blend((z>>8)&255,GetGValue(c),v)<<8|aa_blend(z&255,GetBValue(c),v);
                }
                if(!cpu_text)hd_upload_region(b,l,t,r-l,bot-t);
            }
            x+=f->g[ch].dadv;
        }
        hd_gpu_text_end();
        if(cpu_text)hd_upload_region(b,read_l,read_t,read_r-read_l,read_b-read_t);
        rgn_free(&rg);
        return;
    }
    if (b->bpp > 8) {
        /* Antialiased, as Wine draws on deep surfaces. */
        unsigned tr = GetRValue(c), tg = GetGValue(c), tb = GetBValue(c);
        if (!aa_data)
            load_aa();
        for (i = 0; i < n && f->aa; i++) {
            AaGlyph *g = &f->aa[(unsigned char)s[i]];
            const unsigned char *cov = aa_coverage(g);
            int gx = x + ox + g->x, gy = y + oy + g->y, row, col, k;
            for (k = 0; cov && k < rg.n; k++) {
                RECT gr, a;
                gr.left = gx; gr.top = gy; gr.right = gx + g->w; gr.bottom = gy + g->h;
                if (!rect_intersect(&a, &gr, &rg.r[k]))
                    continue;
                if (a.left < 0) a.left = 0;
                if (a.top < 0) a.top = 0;
                if (a.right > b->w) a.right = b->w;
                if (a.bottom > b->h) a.bottom = b->h;
                for (row = a.top; row < a.bottom; row++)
                    for (col = a.left; col < a.right; col++) {
                        int v = cov[(row - gy) * g->w + (col - gx)];
                        COLORREF d;
                        if (v <= 1)
                            continue;
                        d = surf_get(b, col, row);
                        surf_put(b, col, row, RGB(aa_blend(GetRValue(d), tr, v), aa_blend(GetGValue(d), tg, v),
                                                  aa_blend(GetBValue(d), tb, v)));
                    }
            }
            x += f->g[(unsigned char)s[i]].adv;     /* antialiased glyphs advance as measured */
        }
        rgn_free(&rg);
        return;
    }
    {
        int idx = b->bpp == 8 ? surf_nearest(b, c) : -1;
        DWORD v32 = (DWORD)(GetRValue(c) << 16 | GetGValue(c) << 8 | GetBValue(c));
        for (i = 0; i < n; i++) {
            const Glyph *g = &f->g[(unsigned char)s[i]];
            int gx = x + ox + g->x, gy = y + oy + g->y, row, col, stride = (g->w + 7) / 8, k;
            for (k = 0; k < rg.n; k++) {
                RECT gr, a;
                gr.left = gx; gr.top = gy; gr.right = gx + g->w; gr.bottom = gy + g->h;
                if (!rect_intersect(&a, &gr, &rg.r[k]))
                    continue;
                if (a.left < 0) a.left = 0;
                if (a.top < 0) a.top = 0;
                if (a.right > b->w) a.right = b->w;
                if (a.bottom > b->h) a.bottom = b->h;
                for (row = a.top; row < a.bottom; row++) {
                    const unsigned char *gb = g->bits + (row - gy) * stride;
                    unsigned char *p = bmp_row(b, row);
                    for (col = a.left; col < a.right; col++) {
                        int bx = col - gx;
                        if (!(gb[bx >> 3] & (0x80 >> (bx & 7))))
                            continue;
                        if (idx >= 0)
                            p[col] = (unsigned char)idx;
                        else if (b->bpp == 32)
                            ((DWORD *)p)[col] = v32;
                        else
                            surf_put(b, col, row, c);
                    }
                }
            }
            x += g->dadv;
        }
    }
    rgn_free(&rg);
}

/* Coverage quads also draw opaque badge backgrounds through the text batch. */
static void camera_text_quad(Bitmap *dst,const unsigned char *cov,int sw,int sh,
                             int gx,int gy,int w,int h,int cl,int ct,int cr,int cb,uint32_t rgb,int cpu_text)
{
    int l=gx>cl ? gx : cl,t=gy>ct ? gy : ct,r=gx+w<cr ? gx+w : cr,b=gy+h<cb ? gy+h : cb;
    if(cov && w>0 && h>0 && l<r && t<b &&
       (cpu_text || !hd_gpu_text(dst,cov,sw,sh,gx,gy,w,h,l,t,r,b,rgb))) {
        int px,py;
        /* Preserve the colored-text fallback on devices without MAX blend. */
        if(!cpu_text)hd_read_region(dst,l,t,r-l,b-t);
        for(py=t;py<b;py++)for(px=l;px<r;px++) {
            int v=cov[((py-gy)*sh/h)*sw+(px-gx)*sw/w];
            uint32_t *out=dst->hd+py*dst->w*hd_density+px,z=*out;
            *out=0xff000000u|aa_blend((z>>16)&255,(rgb>>16)&255,v)<<16|
                aa_blend((z>>8)&255,(rgb>>8)&255,v)<<8|aa_blend(z&255,rgb&255,v);
        }
        if(!cpu_text)hd_upload_region(dst,l,t,r-l,b-t);
    }
}

/* Map names and population badges are hinted at the final physical size, instead of being enlarged
 * along with a lower-resolution terrain image. Placement snaps only to physical
 * pixels; it does not jump a whole logical pixel at higher display densities. */
void text_camera_labels(Bitmap *dst,const MapCameraLabel *labels,int count,double height,
                        int origin_x,int origin_y,const RECT *clip,int kind)
{
    int boxed=kind==1;
    int d=hd_density,size=(int)(height*d+.5),face_size,pass,i,ch;
    int cl=clip->left*d,ct=clip->top*d,cr=clip->right*d,cb=clip->bottom*d;
    int cpu_text;
    FontFace *font;
    double scale;
    const unsigned char *palette=hd_palette();
    if(!count || !dst->hd_gpu)return;
    if(!font_data)load_fonts();if(!aa_data)load_aa();
    if(size<MINH)size=MINH;
    face_size=size>MAXH ? MAXH : size;
    font=faces[kind==1 || kind==2 ? 1 : 0][face_size][kind==3][0];
    if(!font || !font->aa)return;
    scale=(double)size/face_size;
    if(cl<0)cl=0;if(ct<0)ct=0;
    if(cr>dst->w*d)cr=dst->w*d;if(cb>dst->h*d)cb=dst->h*d;
    if(cl>=cr || ct>=cb)return;
    cpu_text=!(host_gpu_available()&2);
    hd_gpu_text_begin();
    /* Without colored-text gamma blending, compose the complete label layer
     * in one CPU transfer. A whole-world view can have thousands of glyphs;
     * synchronizing with the GPU for each one makes blink redraws starve input.
     * Keep shadows and badges in this same plane so overlaps retain order. */
    if(cpu_text)hd_read_region(dst,cl,ct,cr-cl,cb-ct);
    /* Put shadows/backgrounds below the glyphs, retaining atlas batching. */
    for(pass=0;pass<2;pass++)for(i=0;i<count;i++) {
        const MapCameraLabel *label=&labels[i];
        int ll=cl,tt=ct,rr=cr,bb=cb;
        int width=text_width(font,label->text,(int)strlen(label->text));
        int shadow=pass || boxed ? 0 : (size/10>0 ? size/10 : 1);
        if(kind==2 && !pass && !(label->flags & 2))continue;
        double x=(origin_x+label->x)*d-(boxed || kind==3 ? 0 : width*scale*.5)+shadow;
        double y=(origin_y+label->y)*d+shadow;
        int color=label->color&255;
        uint32_t rgb=palette[color*3]<<16|palette[color*3+1]<<8|palette[color*3+2];
        if(kind==2) {
            y-=font->tm[0]*scale*.5;
            if(label->flags & 1) {
                int l=(int)ceil((origin_x+label->left)*d),t=(int)ceil((origin_y+label->top)*d);
                int r=(int)floor((origin_x+label->right)*d),b=(int)floor((origin_y+label->bottom)*d);
                if(ll<l)ll=l;if(tt<t)tt=t;if(rr>r)rr=r;if(bb>b)bb=b;
                if(ll>=rr || tt>=bb)continue;
            }
        }
        if(boxed) {
            static const unsigned char solid=16;
            int pad=(int)(height*d/11+.5),l=(int)floor(x+.5),t=(int)floor(y+.5);
            int digits=(int)strlen(label->text),template_width=text_width(font,"888",digits>3 ? 3 : digits);
            int content=width>template_width ? width : template_width;
            int w,h;
            if(pad<1)pad=1;
            w=(int)(content*scale+.5)+4*pad;h=(int)(font->tm[0]*scale+.5)+2*pad;
            if(!pass) {
                camera_text_quad(dst,&solid,1,1,l,t,w,h,cl,ct,cr,cb,0,cpu_text);
                camera_text_quad(dst,&solid,1,1,l+pad,t+pad,w-2*pad,h-2*pad,cl,ct,cr,cb,rgb,cpu_text);
                continue;
            }
            x=l+2*pad+(content-width)*scale*.5;y=t+pad;rgb=0;
        } else if(!pass)rgb=0;
        if(x+width*scale<ll || x>=rr || y+font->tm[0]*scale<tt || y>=bb)continue;
        for(ch=0;label->text[ch];ch++) {
            unsigned c=(unsigned char)label->text[ch];
            AaGlyph *g=&font->aa[c];
            const unsigned char *cov=aa_coverage(g);
            int gx=(int)floor(x+g->x*scale+.5),gy=(int)floor(y+g->y*scale+.5);
            int w=(int)(g->w*scale+.5),h=(int)(g->h*scale+.5);
            camera_text_quad(dst,cov,g->w,g->h,gx,gy,w,h,ll,tt,rr,bb,rgb,cpu_text);
            x+=font->g[c].dadv*scale;
        }
    }
    hd_gpu_text_end();
    if(cpu_text)hd_upload_region(dst,cl,ct,cr-cl,cb-ct);
}

/* An underline (for & prefixes) under characters [from, from+len) of s. */
static void underline(DC *dc, FontFace *f, int x, int y, const char *s, int from, int len, const RECT *clip)
{
    RECT r;
    Pen pen;
    (void)pen;
    r.left = x + text_width(f, s, from);
    r.right = r.left + text_width(f, s + from, len) - 1;     /* Wine stops one pixel short */
    r.top = y + f->tm[1] + 1;
    r.bottom = r.top + 1;
    if (clip) {
        RECT a;
        if (!rect_intersect(&a, &r, clip))
            return;
        r = a;
    }
    dc_fill_rect(dc, &r, dc->text_color);
}

/* ---- GDI entry points ---- */

HFONT WINAPI CreateFontIndirectA(const LOGFONTA *lf)
{ API_TRACE();
    Font *f = calloc(1, sizeof *f);
    f->lf = *lf;
    f->face = font_face_for(lf);
    plat_log("CreateFont(%s, %d, weight %d, italic %d) -> face %d height %d\n", lf->lfFaceName, (int)lf->lfHeight,
             (int)lf->lfWeight, lf->lfItalic, f->face ? f->face->face : -1, f->face ? f->face->height : -1);
    return obj_new(OBJ_FONT, f);
}

WINBOOL WINAPI GetTextMetricsA(HDC h, LPTEXTMETRICA tm)
{ API_TRACE();
    DC *dc = dc_get(h);
    FontFace *f = dc ? dc_face(dc) : NULL;
    if (!f)
        return FALSE;
    text_metrics(f, tm);
    return TRUE;
}

WINBOOL WINAPI GetTextExtentPointA(HDC h, LPCSTR s, int n, LPSIZE sz)
{ API_TRACE();
    DC *dc = dc_get(h);
    FontFace *f = dc ? dc_face(dc) : NULL;
    if (!f)
        return FALSE;
    sz->cx = text_width(f, s, n);
    sz->cy = f->tm[0];
    if (getenv("CIV2_LOG_TEXT"))
        plat_log("extent \"%.*s\" -> %ld x %ld\n", n, s, (long)sz->cx, (long)sz->cy);
    return TRUE;
}

/* Strip & prefixes; *ul = index of the underlined character or -1. */
static int strip_prefix(const char *s, int n, char *out, int *ul)
{
    int i, o = 0;
    *ul = -1;
    for (i = 0; i < n; i++) {
        if (s[i] == '&' && i + 1 < n) {
            i++;
            if (s[i] != '&')
                *ul = o;
        }
        out[o++] = s[i];
    }
    return o;
}

/* TA_UPDATECP: the text goes at the current position, whatever the rect
 * and the alignment flags say (still clipped to the rect), which then
 * moves past it (tools/probe/cpprobe.c). */
static void at_current_position(DC *dc, int *x, int *y, int w)
{
    if (!(dc->text_align & TA_UPDATECP))
        return;
    *x = dc->pos.x;
    *y = dc->pos.y;
    dc->pos.x += w;
}

int WINAPI DrawTextA(HDC h, LPCSTR str, int len, LPRECT rect, UINT fmt)
{ API_TRACE();
    DC *dc = dc_get(h);
    FontFace *f = dc ? dc_face(dc) : NULL;
    int lh, y, maxw = 0, width;
    const char *p, *end;
    char line[1024];
    if (!f || !str)
        return 0;
    if (len < 0)
        len = (int)strlen(str);
    if (getenv("CIV2_LOG_TEXT"))
        plat_log("DrawText(\"%.*s\", face %d h %d bold %d, fmt %x, rect %d,%d,%d,%d)\n", len, str, f->face, f->height,
                 f->bold, fmt, (int)rect->left, (int)rect->top, (int)rect->right, (int)rect->bottom);
    lh = f->tm[0] + ((fmt & DT_EXTERNALLEADING) ? f->tm[4] : 0);
    width = rect->right - rect->left;
    y = rect->top;
    p = str;
    end = str + len;
    if ((fmt & DT_SINGLELINE) && (fmt & (DT_VCENTER | DT_BOTTOM))) {
        if (fmt & DT_VCENTER)
            y = rect->top + (rect->bottom - rect->top) / 2 - lh / 2;     /* as Wine computes it */
        else
            y = rect->bottom - lh;
    }
    do {
        int n = 0, ul = -1, w, x;
        const char *q = p;
        /* One line: up to a newline, or word-wrapped to the width. */
        if (fmt & DT_SINGLELINE) {
            q = end;
        } else {
            while (q < end && *q != '\n' && *q != '\r')
                q++;
        }
        {
            char raw[1024];
            int rn = (int)(q - p) < 1023 ? (int)(q - p) : 1023, i, o = 0;
            for (i = 0; i < rn; i++) {
                if (p[i] == '\t' && (fmt & DT_EXPANDTABS)) {
                    int tabw = 8 * f->tm[5], cur = text_width(f, raw, o);
                    while (o < 1000 && text_width(f, raw, o) < (cur / tabw + 1) * tabw)
                        raw[o++] = ' ';
                    continue;
                }
                raw[o++] = p[i];
            }
            if (fmt & DT_NOPREFIX) {
                memcpy(line, raw, (size_t)o);
                n = o;
            } else {
                n = strip_prefix(raw, o, line, &ul);
            }
        }
        if ((fmt & DT_WORDBREAK) && !(fmt & DT_SINGLELINE) && text_width(f, line, n) > width) {
            /* Break after the last space that fits (or mid-word if none). */
            int fit = 0, last_space = -1, i;
            for (i = 0; i < n; i++) {
                if (text_width(f, line, i + 1) > width)
                    break;
                if (line[i] == ' ')
                    last_space = i;
                fit = i + 1;
            }
            if (i < n) {
                int brk = last_space >= 0 ? last_space : (fit ? fit : 1);
                int consumed = brk, skip = last_space >= 0 ? 1 : 0;
                /* Map back to the source: count source chars giving `consumed` output chars. */
                const char *s = p;
                int o = 0;
                while (s < end && o < consumed) {
                    if (!(fmt & DT_NOPREFIX) && *s == '&' && s + 1 < end)
                        s++;
                    s++;
                    o++;
                }
                n = consumed;
                q = s;
                if (skip)
                    while (q < end && *q == ' ')
                        q++;
                if (ul >= n)
                    ul = -1;
                /* q is the start of the next line: no newline to skip. */
                w = text_width(f, line, n);
                if (w > maxw) maxw = w;
                if (!(fmt & DT_CALCRECT)) {
                    int ty = y;
                    x = rect->left;
                    if (fmt & DT_CENTER) x = (rect->left + rect->right - w) / 2;
                    else if (fmt & DT_RIGHT) x = rect->right - w;
                    at_current_position(dc, &x, &ty, w);
                    text_draw(dc, x, ty, line, n, (fmt & DT_NOCLIP) ? NULL : rect);
                    if (ul >= 0)
                        underline(dc, f, x, ty, line, ul, 1, (fmt & DT_NOCLIP) ? NULL : rect);
                }
                y += lh;
                p = q;
                continue;
            }
        }
        w = text_width(f, line, n);
        if (w > maxw)
            maxw = w;
        if (!(fmt & DT_CALCRECT)) {
            int ty = y;
            x = rect->left;
            if (fmt & DT_CENTER)
                x = (rect->left + rect->right - w) / 2;
            else if (fmt & DT_RIGHT)
                x = rect->right - w;
            at_current_position(dc, &x, &ty, w);
            text_draw(dc, x, ty, line, n, (fmt & DT_NOCLIP) ? NULL : rect);
            if (ul >= 0)
                underline(dc, f, x, ty, line, ul, 1, (fmt & DT_NOCLIP) ? NULL : rect);
        }
        y += lh;
        p = q;
        if (p < end && *p == '\r') p++;
        if (p < end && *p == '\n') p++;
    } while (p < end && !(fmt & DT_SINGLELINE));
    if (fmt & DT_CALCRECT) {
        rect->right = rect->left + maxw;
        rect->bottom = y;
    }
    return y - rect->top;
}

int WINAPI EnumFontFamiliesA(HDC h, LPCSTR family, FONTENUMPROCA proc, LPARAM lp)
{ API_TRACE();
    static const char *names[] = {"DejaVu Sans", "DejaVu Sans Mono", "DejaVu Serif"};
    int i, r = 1;
    (void)h;
    for (i = 0; i < 3 && r; i++) {
        LOGFONTA lf;
        TEXTMETRICA tm;
        if (family && strcasecmp(family, names[i]))
            continue;
        memset(&lf, 0, sizeof lf);
        strcpy(lf.lfFaceName, names[i]);
        lf.lfHeight = -16;
        lf.lfWeight = 400;
        text_metrics(font_face_for(&lf), &tm);
        r = proc(&lf, &tm, TRUETYPE_FONTTYPE, lp);
    }
    return r;
}

int WINAPI AddFontResourceA(LPCSTR file)
{ API_TRACE();
    char host[1024];
    FILE *f;
    plat_host_path(file, host, sizeof host, 0);
    plat_log("AddFontResource(%s)\n", file);
    f = fopen(host, "rb");
    if (!f)
        return 0;
    fclose(f);
    plat_log("AddFontResource(%s): font files are not supported\n", file);
    return 0;
}

WINBOOL WINAPI RemoveFontResourceA(LPCSTR file)
{ API_TRACE();
    (void)file;
    return FALSE;
}
