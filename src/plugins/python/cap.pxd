cdef from extern "cap.h":
    ctypedef enum CapOperation:
        CAP_NONE = 0
        CAP_LS
        CAP_ACK
        CAP_NAK

    ctypedef struct CapState:
        struct server *server
        CapOperation op
        char *caps
        char caps_request[2048]
        int refs

    CapState *cap_state_new(struct server *serv, const gchar *op_token, const gchar *caps)
    void cap_state_ref(CapState *cap)
    void cap_state_unref(CapState *cap)
    void cap_add_cap(CapState *cap, const gchar *token)
    void cap_request(CapState *cap)
