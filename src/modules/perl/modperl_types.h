#ifndef MODPERL_TYPES_H
#define MODPERL_TYPES_H

/* aliases */

typedef apr_array_header_t MpAV;
typedef apr_table_t        MpHV;

/* xs typemap */

typedef request_rec *  Apache;
typedef request_rec *  Apache__SubRequest;
typedef conn_rec    *  Apache__Connection;
typedef server_rec  *  Apache__Server;

typedef cmd_parms   *  Apache__CmdParms;
typedef module      *  Apache__Module;
typedef handler_rec *  Apache__Handler;
typedef command_rec *  Apache__Command;

typedef apr_table_t   * Apache__table;
typedef apr_pool_t    * Apache__Pool;

/* mod_perl structures */

typedef struct {
    request_rec *r;
    conn_rec    *c;
    server_rec  *s;
} modperl_rcs_t;

#ifdef USE_ITHREADS

typedef struct modperl_list_t modperl_list_t;

struct modperl_list_t {
    modperl_list_t *prev, *next;
    void *data;
};

typedef struct modperl_interp_t modperl_interp_t;
typedef struct modperl_interp_pool_t modperl_interp_pool_t;
typedef struct modperl_tipool_t modperl_tipool_t;

struct modperl_interp_t {
    modperl_interp_pool_t *mip;
    PerlInterpreter *perl;
    int num_requests;
    int flags;
#ifdef MP_TRACE
    unsigned long tid;
#endif
};

typedef struct {
    /* s == startup grow
     * r == runtime grow
     */
    void * (*tipool_sgrow)(modperl_tipool_t *tipool, void *data);
    void * (*tipool_rgrow)(modperl_tipool_t *tipool, void *data);
    void (*tipool_shrink)(modperl_tipool_t *tipool, void *data,
                          void *item);
    void (*tipool_destroy)(modperl_tipool_t *tipool, void *data,
                           void *item);
    void (*tipool_dump)(modperl_tipool_t *tipool, void *data,
                        modperl_list_t *listp);
} modperl_tipool_vtbl_t;

typedef struct {
    int start; /* number of items to create at startup */
    int min_spare; /* minimum number of spare items */
    int max_spare; /* maximum number of spare items */
    int max; /* maximum number of items */
    int max_requests; /* maximum number of requests per item */
} modperl_tipool_config_t;

struct modperl_tipool_t {
    perl_mutex tiplock;
    perl_cond available;
    apr_pool_t *ap_pool;
    modperl_list_t *idle, *busy;
    int in_use; /* number of items currrently in use */
    int size; /* current number of items */
    void *data; /* user data */
    modperl_tipool_config_t *cfg;
    modperl_tipool_vtbl_t *func;
};

struct modperl_interp_pool_t {
    apr_pool_t *ap_pool;
    server_rec *server;
    modperl_tipool_t *tipool;
    modperl_tipool_config_t *tipool_cfg;
    modperl_interp_t *parent; /* from which to perl_clone() */
};

#endif /* USE_ITHREADS */

typedef struct {
    MpAV *handlers[MP_PROCESS_NUM_HANDLERS];
} modperl_process_config_t;

typedef struct {
    MpAV *handlers[MP_CONNECTION_NUM_HANDLERS];
} modperl_connection_config_t;

typedef struct {
    MpAV *handlers[MP_FILES_NUM_HANDLERS];
} modperl_files_config_t;

typedef U32 modperl_opts_t;

typedef struct {
    modperl_opts_t opts;
    modperl_opts_t opts_add;
    modperl_opts_t opts_remove;
    modperl_opts_t opts_override;
    int unset;
} modperl_options_t;

typedef struct {
    MpHV *SetVars;
    MpAV *PassEnv;
    MpAV *PerlRequire, *PerlModule;
    MpAV *handlers[MP_PER_SRV_NUM_HANDLERS];
    modperl_files_config_t *files_cfg;
    modperl_process_config_t *process_cfg;
    modperl_connection_config_t *connection_cfg;
#ifdef USE_ITHREADS
    modperl_interp_pool_t *mip;
    modperl_tipool_config_t *interp_pool_cfg;
#else
    PerlInterpreter *perl;
#endif
#ifdef MP_USE_GTOP
    modperl_gtop_t *gtop;
#endif
    MpAV *argv;
    modperl_options_t *flags;
} modperl_srv_config_t;

typedef struct {
    char *location;
    char *PerlDispatchHandler;
    MpAV *handlers[MP_PER_DIR_NUM_HANDLERS];
    MpHV *SetEnv;
    MpHV *SetVars;
    int flags;
} modperl_dir_config_t;

typedef struct {
    SV *obj; /* object or classname if cv is a method */
    SV *cv; /* subroutine reference or name */
    char *name; /* orignal name from .conf if any */
    int cvgen; /* XXX: for caching */
    AV *args; /* XXX: switch to something lighter */
    int flags;
    PerlInterpreter *perl;
} modperl_handler_t;

#define MP_HANDLER_TYPE_CHAR 1
#define MP_HANDLER_TYPE_SV   2

typedef struct {
    int outcnt;
    char outbuf[IOBUFSIZE];
    apr_pool_t *pool;
    ap_filter_t *filters;
} modperl_wbucket_t;

typedef enum {
    MP_INPUT_FILTER_MODE,
    MP_OUTPUT_FILTER_MODE,
} modperl_filter_mode_e;

typedef struct {
    int eos;
    ap_filter_t *f;
    char *leftover;
    apr_ssize_t remaining;
    modperl_wbucket_t wbucket;
    ap_bucket *bucket;
    ap_bucket_brigade *bb;
    apr_status_t rc;
    modperl_filter_mode_e mode;
    apr_pool_t *pool;
} modperl_filter_t;

typedef modperl_filter_t *  Apache__Filter;
typedef modperl_filter_t *  Apache__OutputFilter;
typedef modperl_filter_t *  Apache__InputFilter;

typedef struct {
    SV *data;
    modperl_handler_t *handler;
    PerlInterpreter *perl;
} modperl_filter_ctx_t;

typedef struct {
    HV *pnotes;
    modperl_wbucket_t wbucket;
} modperl_request_config_t;

#endif /* MODPERL_TYPES_H */
