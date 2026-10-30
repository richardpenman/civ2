/*
 * Scope-bound release of what a function acquires (step 6, RAII).
 *
 * Each guard releases at the end of the scope it is declared in, on every
 * path out of it, in the reverse order of declaration.  Where the original
 * released earlier than the end of the function (before a repaint, before
 * a message loop), the guard sits in a nested `{ ... }` that ends there.
 *
 * Only what the game's code uses is here: a function that gets a device
 * context, selects a GDI object, locks global memory, shows the wait cursor
 * or disables a window releases it on a single path, often before more work
 * (a repaint, a message loop), so it stays written out.
 *
 * Everything here is forced inline: the native builds compile with
 * -fno-inline, and an inlined guard gives the same calls, per path, as the
 * release written out by hand on each of them.  The game's own objects
 * (windows, surfaces, dialogs) get no C++ constructors or destructors; a
 * local one is built with its explicit construct call and released with
 * ON_SCOPE_EXIT(obj.destruct()) right after it (what the original's MSVC
 * unwind funclets did).
 */
#ifndef CIV2_SCOPED_H
#define CIV2_SCOPED_H

#define SCOPED_INLINE inline __attribute__((always_inline))

/* Runs F at scope exit (one-off cases).  Usually through ON_SCOPE_EXIT. */
template <class F>
class ScopeExit {
public:
    SCOPED_INLINE explicit ScopeExit(F f) : f_(f) {}
    SCOPED_INLINE ~ScopeExit() { f_(); }
    ScopeExit(const ScopeExit &) = delete;
    ScopeExit &operator=(const ScopeExit &) = delete;
private:
    F f_;
};

template <class F>
static SCOPED_INLINE ScopeExit<F> scope_exit(F f) { return ScopeExit<F>(f); }

#define SCOPED_CAT2(a, b) a##b
#define SCOPED_CAT(a, b) SCOPED_CAT2(a, b)
/* ON_SCOPE_EXIT(dlg.Dialog::destruct());  runs the statement at scope exit */
#define ON_SCOPE_EXIT(...) \
    auto SCOPED_CAT(scope_exit_, __LINE__) = scope_exit([&]() __attribute__((always_inline)) { __VA_ARGS__; })

/* A C runtime file (_fopen): closed at scope exit if open. */
class ScopedFile {
public:
    SCOPED_INLINE explicit ScopedFile(FILE *f) : f_(f) {}
    SCOPED_INLINE ~ScopedFile() { if (f_ != (FILE *)0x0) _fclose(f_); }
    SCOPED_INLINE operator FILE *() const { return f_; }
    /* closes it now: 0, or EOF if what was still buffered could not be written */
    SCOPED_INLINE int close() { int rc = f_ != (FILE *)0x0 ? _fclose(f_) : 0; f_ = (FILE *)0x0; return rc; }
    SCOPED_INLINE explicit operator iptr() const { return (iptr)f_; }   /* (iptr)file */
    ScopedFile(const ScopedFile &) = delete;
    ScopedFile &operator=(const ScopedFile &) = delete;
private:
    FILE *f_;
};

#endif
