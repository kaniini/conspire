cdef from extern "signal_factory.h":
    ctypedef struct Signal:
        char *name
        gboolean stop
        GList *handlers
        gpointer *values

    ctypedef void (*SignalHandler)(gpointer *params);

    void signal_attach(const gchar *signal, SignalHandler hdl);
    void signal_attach_head(const gchar *signal, SignalHandler hdl);

    gint signal_emit(const gchar *signal, int params, ...);
    void signal_continue(int params, ...);

    void signal_stop(const gchar *signal);
    const gchar *signal_get_current_name(void);

    void signal_disconnect(const gchar *signal, SignalHandler hdl);

