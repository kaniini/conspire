cdef from extern "linequeue.h":
    typedef int (*LineQueueWriter)(gpointer data, gchar *line, gint len)
    typedef int (*LineQueueUpdater)(gpointer data)

    ctypedef struct LineQueue:
        GQueue *q
        void *data
        LineQueueWriter w
        LineQueueUpdater update
        int available
        int writeoffs

    LineQueue *linequeue_new(gpointer data, LineQueueWriter w, LineQueueUpdater u)
    void linequeue_add_line(LineQueue *lq, gchar *line)
    void linequeue_flush(LineQueue *lq)
    void linequeue_destroy(LineQueue *lq)
    void linequeue_erase(LineQueue *lq)

    static inline gint
    linequeue_size(LineQueue *lq):
        if lq is NULL:
            return -1

	return (gint) g_queue_get_length(&lq->q);

