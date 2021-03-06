/*
The MIT License (MIT)

Copyright (c) 2013-2015 SRS(ossrs)

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <signal.h>
#include <srs_app_http_api.hpp>

#ifdef SRS_AUTO_HTTP_API

#include <sstream>
#include <stdlib.h>
#include <vector>
using namespace std;

#include <srs_kernel_log.hpp>
#include <srs_kernel_error.hpp>
#include <srs_app_st.hpp>
#include <srs_core_autofree.hpp>
#include <srs_protocol_json.hpp>
#include <srs_kernel_utility.hpp>
#include <srs_app_utility.hpp>
#include <srs_app_statistic.hpp>
#include <srs_rtmp_stack.hpp>
#include <srs_app_dvr.hpp>
#include <srs_app_config.hpp>
#include <srs_app_source.hpp>
#include <srs_app_http_conn.hpp>

int srs_api_response_jsonp(ISrsHttpResponseWriter* w, string callback, string data)
{
    int ret = ERROR_SUCCESS;
    
    SrsHttpHeader* h = w->header();
    
    h->set_content_length(data.length() + callback.length() + 2);
    h->set_content_type("text/javascript");
    
    if (!callback.empty() && (ret = w->write((char*)callback.data(), (int)callback.length())) != ERROR_SUCCESS) {
        return ret;
    }
    
    static char* c0 = (char*)"(";
    if ((ret = w->write(c0, 1)) != ERROR_SUCCESS) {
        return ret;
    }
    if ((ret = w->write((char*)data.data(), (int)data.length())) != ERROR_SUCCESS) {
        return ret;
    }
    
    static char* c1 = (char*)")";
    if ((ret = w->write(c1, 1)) != ERROR_SUCCESS) {
        return ret;
    }
    
    return ret;
}

int srs_api_response_jsonp_code(ISrsHttpResponseWriter* w, string callback, int code)
{
    std::stringstream ss;
    
    ss << SRS_JOBJECT_START
            << SRS_JFIELD_ERROR(code)
        << SRS_JOBJECT_END;
    
    return srs_api_response_jsonp(w, callback, ss.str());
}

int srs_api_response_json(ISrsHttpResponseWriter* w, string data)
{
    SrsHttpHeader* h = w->header();
    
    h->set_content_length(data.length());
    h->set_content_type("application/json");
    
    return w->write((char*)data.data(), (int)data.length());
}

int srs_api_response_json_code(ISrsHttpResponseWriter* w, int code)
{
    std::stringstream ss;
    
    ss << SRS_JOBJECT_START
            << SRS_JFIELD_ERROR(code)
        << SRS_JOBJECT_END;
    
    return srs_api_response_json(w, ss.str());
}

int srs_api_response(ISrsHttpResponseWriter* w, ISrsHttpMessage* r, std::string json)
{
    // no jsonp, directly response.
    if (!r->is_jsonp()) {
        return srs_api_response_json(w, json);
    }
    
    // jsonp, get function name from query("callback")
    string callback = r->query_get("callback");
    return srs_api_response_jsonp(w, callback, json);
}

int srs_api_response_code(ISrsHttpResponseWriter* w, ISrsHttpMessage* r, int code)
{
    // no jsonp, directly response.
    if (!r->is_jsonp()) {
        return srs_api_response_json_code(w, code);
    }
    
    // jsonp, get function name from query("callback")
    string callback = r->query_get("callback");
    return srs_api_response_jsonp_code(w, callback, code);
}

SrsGoApiRoot::SrsGoApiRoot()
{
}

SrsGoApiRoot::~SrsGoApiRoot()
{
}

int SrsGoApiRoot::serve_http(ISrsHttpResponseWriter* w, ISrsHttpMessage* r)
{
    SrsStatistic* stat = SrsStatistic::instance();
    std::stringstream ss;
    
    ss << SRS_JOBJECT_START
        << SRS_JFIELD_ERROR(ERROR_SUCCESS) << SRS_JFIELD_CONT
        << SRS_JFIELD_ORG("server", stat->server_id()) << SRS_JFIELD_CONT
        << SRS_JFIELD_ORG("urls", SRS_JOBJECT_START)
            << SRS_JFIELD_STR("api", "the api root")
        << SRS_JOBJECT_END
        << SRS_JOBJECT_END;
        
    return srs_api_response(w, r, ss.str());
}

SrsGoApiApi::SrsGoApiApi()
{
}

SrsGoApiApi::~SrsGoApiApi()
{
}

int SrsGoApiApi::serve_http(ISrsHttpResponseWriter* w, ISrsHttpMessage* r)
{
    SrsStatistic* stat = SrsStatistic::instance();
    std::stringstream ss;
    
    ss << SRS_JOBJECT_START
        << SRS_JFIELD_ERROR(ERROR_SUCCESS) << SRS_JFIELD_CONT
        << SRS_JFIELD_ORG("server", stat->server_id()) << SRS_JFIELD_CONT
        << SRS_JFIELD_ORG("urls", SRS_JOBJECT_START)
            << SRS_JFIELD_STR("v1", "the api version 1.0")
        << SRS_JOBJECT_END
        << SRS_JOBJECT_END;
        
    return srs_api_response(w, r, ss.str());
}

SrsGoApiV1::SrsGoApiV1()
{
}

SrsGoApiV1::~SrsGoApiV1()
{
}

int SrsGoApiV1::serve_http(ISrsHttpResponseWriter* w, ISrsHttpMessage* r)
{
    SrsStatistic* stat = SrsStatistic::instance();
    std::stringstream ss;
    
    ss << SRS_JOBJECT_START
        << SRS_JFIELD_ERROR(ERROR_SUCCESS) << SRS_JFIELD_CONT
        << SRS_JFIELD_ORG("server", stat->server_id()) << SRS_JFIELD_CONT
        << SRS_JFIELD_ORG("urls", SRS_JOBJECT_START)
            << SRS_JFIELD_STR("versions", "the version of SRS") << SRS_JFIELD_CONT
            << SRS_JFIELD_STR("summaries", "the summary(pid, argv, pwd, cpu, mem) of SRS") << SRS_JFIELD_CONT
            << SRS_JFIELD_STR("rusages", "the rusage of SRS") << SRS_JFIELD_CONT
            << SRS_JFIELD_STR("self_proc_stats", "the self process stats") << SRS_JFIELD_CONT
            << SRS_JFIELD_STR("system_proc_stats", "the system process stats") << SRS_JFIELD_CONT
            << SRS_JFIELD_STR("meminfos", "the meminfo of system") << SRS_JFIELD_CONT
            << SRS_JFIELD_STR("authors", "the license, copyright, authors and contributors") << SRS_JFIELD_CONT
            << SRS_JFIELD_STR("features", "the supported features of SRS") << SRS_JFIELD_CONT
            << SRS_JFIELD_STR("requests", "the request itself, for http debug") << SRS_JFIELD_CONT
            << SRS_JFIELD_STR("vhosts", "manage all vhosts or specified vhost") << SRS_JFIELD_CONT
            << SRS_JFIELD_STR("streams", "manage all streams or specified stream") << SRS_JFIELD_CONT
            << SRS_JFIELD_STR("clients", "manage all clients or specified client, default query top 10 clients") << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("tests", SRS_JOBJECT_START)
                << SRS_JFIELD_STR("requests", "show the request info") << SRS_JFIELD_CONT
                << SRS_JFIELD_STR("errors", "always return an error 100") << SRS_JFIELD_CONT
                << SRS_JFIELD_STR("redirects", "always redirect to /api/v1/test/errors") << SRS_JFIELD_CONT
                << SRS_JFIELD_STR("[vhost]", "http vhost for http://error.srs.com:1985/api/v1/tests/errors")
            << SRS_JOBJECT_END
        << SRS_JOBJECT_END
        << SRS_JOBJECT_END;
    
    return srs_api_response(w, r, ss.str());
}

SrsGoApiVersion::SrsGoApiVersion()
{
}

SrsGoApiVersion::~SrsGoApiVersion()
{
}

int SrsGoApiVersion::serve_http(ISrsHttpResponseWriter* w, ISrsHttpMessage* r)
{
    SrsStatistic* stat = SrsStatistic::instance();
    std::stringstream ss;
    
    ss << SRS_JOBJECT_START
        << SRS_JFIELD_ERROR(ERROR_SUCCESS) << SRS_JFIELD_CONT
        << SRS_JFIELD_ORG("server", stat->server_id()) << SRS_JFIELD_CONT
        << SRS_JFIELD_ORG("data", SRS_JOBJECT_START)
            << SRS_JFIELD_ORG("major", VERSION_MAJOR) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("minor", VERSION_MINOR) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("revision", VERSION_REVISION) << SRS_JFIELD_CONT
            << SRS_JFIELD_STR("version", RTMP_SIG_SRS_VERSION)
        << SRS_JOBJECT_END
        << SRS_JOBJECT_END;
    
    return srs_api_response(w, r, ss.str());
}

SrsGoApiSummaries::SrsGoApiSummaries()
{
}

SrsGoApiSummaries::~SrsGoApiSummaries()
{
}

int SrsGoApiSummaries::serve_http(ISrsHttpResponseWriter* w, ISrsHttpMessage* r)
{
    std::stringstream ss;
    srs_api_dump_summaries(ss);
    return srs_api_response(w, r, ss.str());
}

SrsGoApiRusages::SrsGoApiRusages()
{
}

SrsGoApiRusages::~SrsGoApiRusages()
{
}

int SrsGoApiRusages::serve_http(ISrsHttpResponseWriter* w, ISrsHttpMessage* r)
{
    SrsStatistic* stat = SrsStatistic::instance();
    std::stringstream ss;
    
    SrsRusage* ru = srs_get_system_rusage();
    
    ss << SRS_JOBJECT_START
        << SRS_JFIELD_ERROR(ERROR_SUCCESS) << SRS_JFIELD_CONT
        << SRS_JFIELD_ORG("server", stat->server_id()) << SRS_JFIELD_CONT
        << SRS_JFIELD_ORG("data", SRS_JOBJECT_START)
            << SRS_JFIELD_ORG("ok", (ru->ok? "true":"false")) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("sample_time", ru->sample_time) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("ru_utime", ru->r.ru_utime.tv_sec) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("ru_stime", ru->r.ru_stime.tv_sec) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("ru_maxrss", ru->r.ru_maxrss) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("ru_ixrss", ru->r.ru_ixrss) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("ru_idrss", ru->r.ru_idrss) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("ru_isrss", ru->r.ru_isrss) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("ru_minflt", ru->r.ru_minflt) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("ru_majflt", ru->r.ru_majflt) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("ru_nswap", ru->r.ru_nswap) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("ru_inblock", ru->r.ru_inblock) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("ru_oublock", ru->r.ru_oublock) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("ru_msgsnd", ru->r.ru_msgsnd) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("ru_msgrcv", ru->r.ru_msgrcv) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("ru_nsignals", ru->r.ru_nsignals) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("ru_nvcsw", ru->r.ru_nvcsw) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("ru_nivcsw", ru->r.ru_nivcsw)
        << SRS_JOBJECT_END
        << SRS_JOBJECT_END;
    
    return srs_api_response(w, r, ss.str());
}

SrsGoApiSelfProcStats::SrsGoApiSelfProcStats()
{
}

SrsGoApiSelfProcStats::~SrsGoApiSelfProcStats()
{
}

int SrsGoApiSelfProcStats::serve_http(ISrsHttpResponseWriter* w, ISrsHttpMessage* r)
{
    SrsStatistic* stat = SrsStatistic::instance();
    std::stringstream ss;
    
    SrsProcSelfStat* u = srs_get_self_proc_stat();
    
    ss << SRS_JOBJECT_START
        << SRS_JFIELD_ERROR(ERROR_SUCCESS) << SRS_JFIELD_CONT
        << SRS_JFIELD_ORG("server", stat->server_id()) << SRS_JFIELD_CONT
        << SRS_JFIELD_ORG("data", SRS_JOBJECT_START)
            << SRS_JFIELD_ORG("ok", (u->ok? "true":"false")) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("sample_time", u->sample_time) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("percent", u->percent) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("pid", u->pid) << SRS_JFIELD_CONT
            << SRS_JFIELD_STR("comm", u->comm) << SRS_JFIELD_CONT
            << SRS_JFIELD_STR("state", u->state) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("ppid", u->ppid) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("pgrp", u->pgrp) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("session", u->session) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("tty_nr", u->tty_nr) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("tpgid", u->tpgid) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("flags", u->flags) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("minflt", u->minflt) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("cminflt", u->cminflt) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("majflt", u->majflt) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("cmajflt", u->cmajflt) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("utime", u->utime) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("stime", u->stime) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("cutime", u->cutime) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("cstime", u->cstime) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("priority", u->priority) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("nice", u->nice) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("num_threads", u->num_threads) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("itrealvalue", u->itrealvalue) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("starttime", u->starttime) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("vsize", u->vsize) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("rss", u->rss) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("rsslim", u->rsslim) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("startcode", u->startcode) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("endcode", u->endcode) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("startstack", u->startstack) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("kstkesp", u->kstkesp) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("kstkeip", u->kstkeip) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("signal", u->signal) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("blocked", u->blocked) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("sigignore", u->sigignore) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("sigcatch", u->sigcatch) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("wchan", u->wchan) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("nswap", u->nswap) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("cnswap", u->cnswap) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("exit_signal", u->exit_signal) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("processor", u->processor) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("rt_priority", u->rt_priority) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("policy", u->policy) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("delayacct_blkio_ticks", u->delayacct_blkio_ticks) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("guest_time", u->guest_time) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("cguest_time", u->cguest_time)
        << SRS_JOBJECT_END
        << SRS_JOBJECT_END;
    
    return srs_api_response(w, r, ss.str());
}

SrsGoApiSystemProcStats::SrsGoApiSystemProcStats()
{
}

SrsGoApiSystemProcStats::~SrsGoApiSystemProcStats()
{
}

int SrsGoApiSystemProcStats::serve_http(ISrsHttpResponseWriter* w, ISrsHttpMessage* r)
{
    SrsStatistic* stat = SrsStatistic::instance();
    std::stringstream ss;
    
    SrsProcSystemStat* s = srs_get_system_proc_stat();
    
    ss << SRS_JOBJECT_START
        << SRS_JFIELD_ERROR(ERROR_SUCCESS) << SRS_JFIELD_CONT
        << SRS_JFIELD_ORG("server", stat->server_id()) << SRS_JFIELD_CONT
        << SRS_JFIELD_ORG("data", SRS_JOBJECT_START)
            << SRS_JFIELD_ORG("ok", (s->ok? "true":"false")) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("sample_time", s->sample_time) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("percent", s->percent) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("user", s->user) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("nice", s->nice) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("sys", s->sys) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("idle", s->idle) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("iowait", s->iowait) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("irq", s->irq) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("softirq", s->softirq) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("steal", s->steal) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("guest", s->guest)
        << SRS_JOBJECT_END
        << SRS_JOBJECT_END;
    
    return srs_api_response(w, r, ss.str());
}

SrsGoApiMemInfos::SrsGoApiMemInfos()
{
}

SrsGoApiMemInfos::~SrsGoApiMemInfos()
{
}

int SrsGoApiMemInfos::serve_http(ISrsHttpResponseWriter* w, ISrsHttpMessage* r)
{
    SrsStatistic* stat = SrsStatistic::instance();
    std::stringstream ss;
    
    SrsMemInfo* m = srs_get_meminfo();
    
    ss << SRS_JOBJECT_START
        << SRS_JFIELD_ERROR(ERROR_SUCCESS) << SRS_JFIELD_CONT
        << SRS_JFIELD_ORG("server", stat->server_id()) << SRS_JFIELD_CONT
        << SRS_JFIELD_ORG("data", SRS_JOBJECT_START)
            << SRS_JFIELD_ORG("ok", (m->ok? "true":"false")) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("sample_time", m->sample_time) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("percent_ram", m->percent_ram) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("percent_swap", m->percent_swap) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("MemActive", m->MemActive) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("RealInUse", m->RealInUse) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("NotInUse", m->NotInUse) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("MemTotal", m->MemTotal) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("MemFree", m->MemFree) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("Buffers", m->Buffers) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("Cached", m->Cached) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("SwapTotal", m->SwapTotal) << SRS_JFIELD_CONT
            << SRS_JFIELD_ORG("SwapFree", m->SwapFree)
        << SRS_JOBJECT_END
        << SRS_JOBJECT_END;
    
    return srs_api_response(w, r, ss.str());
}

SrsGoApiAuthors::SrsGoApiAuthors()
{
}

SrsGoApiAuthors::~SrsGoApiAuthors()
{
}

int SrsGoApiAuthors::serve_http(ISrsHttpResponseWriter* w, ISrsHttpMessage* r)
{
    SrsStatistic* stat = SrsStatistic::instance();
    std::stringstream ss;
    
    ss << SRS_JOBJECT_START
        << SRS_JFIELD_ERROR(ERROR_SUCCESS) << SRS_JFIELD_CONT
        << SRS_JFIELD_ORG("server", stat->server_id()) << SRS_JFIELD_CONT
        << SRS_JFIELD_ORG("data", SRS_JOBJECT_START)
            << SRS_JFIELD_STR("primary", RTMP_SIG_SRS_PRIMARY) << SRS_JFIELD_CONT
            << SRS_JFIELD_STR("license", RTMP_SIG_SRS_LICENSE) << SRS_JFIELD_CONT
            << SRS_JFIELD_STR("copyright", RTMP_SIG_SRS_COPYRIGHT) << SRS_JFIELD_CONT
            << SRS_JFIELD_STR("authors", RTMP_SIG_SRS_AUTHROS) << SRS_JFIELD_CONT
            << SRS_JFIELD_STR("contributors_link", RTMP_SIG_SRS_CONTRIBUTORS_URL) << SRS_JFIELD_CONT
            << SRS_JFIELD_STR("contributors", SRS_AUTO_CONSTRIBUTORS)
        << SRS_JOBJECT_END
        << SRS_JOBJECT_END;
    
    return srs_api_response(w, r, ss.str());
}

SrsGoApiFeatures::SrsGoApiFeatures()
{
}

SrsGoApiFeatures::~SrsGoApiFeatures()
{
}

int SrsGoApiFeatures::serve_http(ISrsHttpResponseWriter* w, ISrsHttpMessage* r)
{
    SrsStatistic* stat = SrsStatistic::instance();
    std::stringstream ss;
    
#ifdef SRS_AUTO_SSL
    bool ssl = true;
#else
    bool ssl = false;
#endif
#ifdef SRS_AUTO_HLS
    bool hls = true;
#else
    bool hls = false;
#endif
#ifdef SRS_AUTO_HDS
    bool hds = true;
#else
    bool hds = false;
#endif
#ifdef SRS_AUTO_HTTP_CALLBACK
    bool callback = true;
#else
    bool callback = false;
#endif
#ifdef SRS_AUTO_HTTP_API
    bool api = true;
#else
    bool api = false;
#endif
#ifdef SRS_AUTO_HTTP_SERVER
    bool httpd = true;
#else
    bool httpd = false;
#endif
#ifdef SRS_AUTO_DVR
    bool dvr = true;
#else
    bool dvr = false;
#endif
#ifdef SRS_AUTO_TRANSCODE
    bool transcode = true;
#else
    bool transcode = false;
#endif
#ifdef SRS_AUTO_INGEST
    bool ingest = true;
#else
    bool ingest = false;
#endif
#ifdef SRS_AUTO_STAT
    bool _stat = true;
#else
    bool _stat = false;
#endif
#ifdef SRS_AUTO_NGINX
    bool nginx = true;
#else
    bool nginx = false;
#endif
#ifdef SRS_AUTO_FFMPEG_TOOL
    bool ffmpeg = true;
#else
    bool ffmpeg = false;
#endif
#ifdef SRS_AUTO_STREAM_CASTER
    bool caster = true;
#else
    bool caster = false;
#endif
#ifdef SRS_PERF_COMPLEX_SEND
    bool complex_send = true;
#else
    bool complex_send = false;
#endif
#ifdef SRS_PERF_TCP_NODELAY
    bool tcp_nodelay = true;
#else
    bool tcp_nodelay = false;
#endif
#ifdef SRS_PERF_SO_SNDBUF_SIZE
    bool so_sendbuf = true;
#else
    bool so_sendbuf = false;
#endif
#ifdef SRS_PERF_MERGED_READ
    bool mr = true;
#else
    bool mr = false;
#endif
    
    ss << SRS_JOBJECT_START
        << SRS_JFIELD_ERROR(ERROR_SUCCESS) << SRS_JFIELD_CONT
        << SRS_JFIELD_ORG("server", stat->server_id()) << SRS_JFIELD_CONT
        << SRS_JFIELD_ORG("data", SRS_JOBJECT_START)
            << SRS_JFIELD_BOOL("ssl", ssl) << SRS_JFIELD_CONT
            << SRS_JFIELD_BOOL("hls", hls) << SRS_JFIELD_CONT
            << SRS_JFIELD_BOOL("hds", hds) << SRS_JFIELD_CONT
            << SRS_JFIELD_BOOL("callback", callback) << SRS_JFIELD_CONT
            << SRS_JFIELD_BOOL("api", api) << SRS_JFIELD_CONT
            << SRS_JFIELD_BOOL("httpd", httpd) << SRS_JFIELD_CONT
            << SRS_JFIELD_BOOL("dvr", dvr) << SRS_JFIELD_CONT
            << SRS_JFIELD_BOOL("transcode", transcode) << SRS_JFIELD_CONT
            << SRS_JFIELD_BOOL("ingest", ingest) << SRS_JFIELD_CONT
            << SRS_JFIELD_BOOL("stat", _stat) << SRS_JFIELD_CONT
            << SRS_JFIELD_BOOL("nginx", nginx) << SRS_JFIELD_CONT
            << SRS_JFIELD_BOOL("ffmpeg", ffmpeg) << SRS_JFIELD_CONT
            << SRS_JFIELD_BOOL("stream_caster", caster) << SRS_JFIELD_CONT
            << SRS_JFIELD_BOOL("complex_send", complex_send) << SRS_JFIELD_CONT
            << SRS_JFIELD_BOOL("tcp_nodelay", tcp_nodelay) << SRS_JFIELD_CONT
            << SRS_JFIELD_BOOL("so_sendbuf", so_sendbuf) << SRS_JFIELD_CONT
            << SRS_JFIELD_BOOL("mr", mr)
        << SRS_JOBJECT_END
        << SRS_JOBJECT_END;
    
    return srs_api_response(w, r, ss.str());
}

SrsGoApiRequests::SrsGoApiRequests()
{
}

SrsGoApiRequests::~SrsGoApiRequests()
{
}

int SrsGoApiRequests::serve_http(ISrsHttpResponseWriter* w, ISrsHttpMessage* r)
{
    ISrsHttpMessage* req = r;
    
    SrsStatistic* stat = SrsStatistic::instance();
    std::stringstream ss;
    
    ss << SRS_JOBJECT_START
        << SRS_JFIELD_ERROR(ERROR_SUCCESS) << SRS_JFIELD_CONT
        << SRS_JFIELD_ORG("server", stat->server_id()) << SRS_JFIELD_CONT
        << SRS_JFIELD_ORG("data", SRS_JOBJECT_START)
            << SRS_JFIELD_STR("uri", req->uri()) << SRS_JFIELD_CONT
            << SRS_JFIELD_STR("path", req->path()) << SRS_JFIELD_CONT;
    
    // method
    if (req->is_http_get()) {
        ss  << SRS_JFIELD_STR("METHOD", "GET");
    } else if (req->is_http_post()) {
        ss  << SRS_JFIELD_STR("METHOD", "POST");
    } else if (req->is_http_put()) {
        ss  << SRS_JFIELD_STR("METHOD", "PUT");
    } else if (req->is_http_delete()) {
        ss  << SRS_JFIELD_STR("METHOD", "DELETE");
    } else {
        ss  << SRS_JFIELD_ORG("METHOD", req->method());
    }
    ss << SRS_JFIELD_CONT;
    
    // request headers
    ss      << SRS_JFIELD_NAME("headers") << SRS_JOBJECT_START;
    for (int i = 0; i < req->request_header_count(); i++) {
        std::string key = req->request_header_key_at(i);
        std::string value = req->request_header_value_at(i);
        if ( i < req->request_header_count() - 1) {
            ss      << SRS_JFIELD_STR(key, value) << SRS_JFIELD_CONT;
        } else {
            ss      << SRS_JFIELD_STR(key, value);
        }
    }
    ss      << SRS_JOBJECT_END << SRS_JFIELD_CONT;
    
    // server informations
    ss      << SRS_JFIELD_NAME("server") << SRS_JOBJECT_START
                << SRS_JFIELD_STR("sigature", RTMP_SIG_SRS_KEY) << SRS_JFIELD_CONT
                << SRS_JFIELD_STR("name", RTMP_SIG_SRS_NAME) << SRS_JFIELD_CONT
                << SRS_JFIELD_STR("version", RTMP_SIG_SRS_VERSION) << SRS_JFIELD_CONT
                << SRS_JFIELD_STR("link", RTMP_SIG_SRS_URL) << SRS_JFIELD_CONT
                << SRS_JFIELD_ORG("time", srs_get_system_time_ms())
            << SRS_JOBJECT_END
        << SRS_JOBJECT_END
        << SRS_JOBJECT_END;
    
    return srs_api_response(w, r, ss.str());
}

SrsGoApiVhosts::SrsGoApiVhosts()
{
}

SrsGoApiVhosts::~SrsGoApiVhosts()
{
}

int SrsGoApiVhosts::serve_http(ISrsHttpResponseWriter* w, ISrsHttpMessage* r)
{
    int ret = ERROR_SUCCESS;
    
    SrsStatistic* stat = SrsStatistic::instance();
    std::stringstream ss;
    
    // path: {pattern}{vhost_id}
    // e.g. /api/v1/vhosts/100     pattern= /api/v1/vhosts/, vhost_id=100
    int vid = r->parse_rest_id(entry->pattern);
    SrsStatisticVhost* vhost = NULL;
    
    if (vid > 0 && (vhost = stat->find_vhost(vid)) == NULL) {
        ret = ERROR_RTMP_STREAM_NOT_FOUND;
        srs_error("vhost id=%d not found. ret=%d", vid, ret);
        return srs_api_response_code(w, r, ret);
    }
    
    if (r->is_http_get()) {
        std::stringstream data;
        
        if (!vhost) {
            ret = stat->dumps_vhosts(data);
            
            ss << SRS_JOBJECT_START
                    << SRS_JFIELD_ERROR(ret) << SRS_JFIELD_CONT
                    << SRS_JFIELD_ORG("server", stat->server_id()) << SRS_JFIELD_CONT
                    << SRS_JFIELD_ORG("vhosts", data.str())
                << SRS_JOBJECT_END;
        } else {
            ret = vhost->dumps(data);
            
            ss << SRS_JOBJECT_START
                    << SRS_JFIELD_ERROR(ret) << SRS_JFIELD_CONT
                    << SRS_JFIELD_ORG("server", stat->server_id()) << SRS_JFIELD_CONT
                    << SRS_JFIELD_ORG("vhost", data.str())
                << SRS_JOBJECT_END;
        }
        
        return srs_api_response(w, r, ss.str());
    }
    
    return srs_api_response(w, r, ss.str());
}

SrsGoApiStreams::SrsGoApiStreams()
{
}

SrsGoApiStreams::~SrsGoApiStreams()
{
}

int SrsGoApiStreams::serve_http(ISrsHttpResponseWriter* w, ISrsHttpMessage* r)
{
    int ret = ERROR_SUCCESS;
    
    SrsStatistic* stat = SrsStatistic::instance();
    std::stringstream ss;
    
    // path: {pattern}{stream_id}
    // e.g. /api/v1/streams/100     pattern= /api/v1/streams/, stream_id=100
    int sid = r->parse_rest_id(entry->pattern);
    
    SrsStatisticStream* stream = NULL;
    if (sid >= 0 && (stream = stat->find_stream(sid)) == NULL) {
        ret = ERROR_RTMP_STREAM_NOT_FOUND;
        srs_error("stream stream_id=%d not found. ret=%d", sid, ret);
        return srs_api_response_code(w, r, ret);
    }
    
    if (r->is_http_get()) {
        std::stringstream data;
        
        if (!stream) {
            ret = stat->dumps_streams(data);
            
            ss << SRS_JOBJECT_START
                    << SRS_JFIELD_ERROR(ret) << SRS_JFIELD_CONT
                    << SRS_JFIELD_ORG("server", stat->server_id()) << SRS_JFIELD_CONT
                    << SRS_JFIELD_ORG("streams", data.str())
                << SRS_JOBJECT_END;
        } else {
            ret = stream->dumps(data);
            
            ss << SRS_JOBJECT_START
                    << SRS_JFIELD_ERROR(ret) << SRS_JFIELD_CONT
                    << SRS_JFIELD_ORG("server", stat->server_id()) << SRS_JFIELD_CONT
                    << SRS_JFIELD_ORG("stream", data.str())
                << SRS_JOBJECT_END;
        }
        
        return srs_api_response(w, r, ss.str());
    }
    
    return ret;
}

SrsGoApiClients::SrsGoApiClients()
{
}

SrsGoApiClients::~SrsGoApiClients()
{
}

int SrsGoApiClients::serve_http(ISrsHttpResponseWriter* w, ISrsHttpMessage* r)
{
    int ret = ERROR_SUCCESS;
    
    SrsStatistic* stat = SrsStatistic::instance();
    std::stringstream ss;
	
    // url -> SrsStatisticStream -> cid -> SrsStatisticClient
    // path: {pattern}{client_id}
    // e.g. /api/v1/clients/live/livestream pattern= /api/v1/clients/, key=/live/livestream cid=107
    
    std::string key;
	ret = r->parse_rest_str(entry->pattern, key);
	if(ret < 0) {
        ret = ERROR_RTMP_CLIENT_NOT_FOUND;
        srs_error("client not found. ret=%d", ret);
        return srs_api_response_code(w, r, ret);		
	}
	
	SrsStatisticStream *stream = stat->find_stream(key);
	if(stream == NULL) {
        ret = ERROR_RTMP_CLIENT_NOT_FOUND;
        srs_error("client url=%s not found. ret=%d", key.c_str(), ret);
        return srs_api_response_code(w, r, ret);			
	}
		
    int cid = stream->connection_cid;
	
    SrsStatisticClient* client = NULL;
    if (cid >= 0 && (client = stat->find_client(cid)) == NULL) {
        ret = ERROR_RTMP_CLIENT_NOT_FOUND;
        srs_error("client id=%d not found. ret=%d", cid, ret);
        return srs_api_response_code(w, r, ret);
    }
    
    if (r->is_http_delete()) {
        if (!client) {
            ret = ERROR_RTMP_CLIENT_NOT_FOUND;
            srs_error("client id=%d not found. ret=%d", cid, ret);
            return srs_api_response_code(w, r, ret);
        }
        
        //client->conn->expire();
		        
		// stop rtmp connection immediately.
        client->conn->dispose();
		
        srs_warn("kickoff client id=%d ok", cid);
        return srs_api_response_code(w, r, ret);
    } else if (r->is_http_get()) {
        std::stringstream data;
        
        if (!client) {
            ret = stat->dumps_clients(data, 0, 10);
            
            ss << SRS_JOBJECT_START
                    << SRS_JFIELD_ERROR(ret) << SRS_JFIELD_CONT
                    << SRS_JFIELD_ORG("server", stat->server_id()) << SRS_JFIELD_CONT
                    << SRS_JFIELD_ORG("clients", data.str())
                << SRS_JOBJECT_END;
        } else {
            ret = client->dumps(data);
            
            ss << SRS_JOBJECT_START
                    << SRS_JFIELD_ERROR(ret) << SRS_JFIELD_CONT
                    << SRS_JFIELD_ORG("server", stat->server_id()) << SRS_JFIELD_CONT
                    << SRS_JFIELD_ORG("client", data.str())
                << SRS_JOBJECT_END;
        }
        
        return srs_api_response(w, r, ss.str());
    } else {
        return srs_go_http_error(w, SRS_CONSTS_HTTP_MethodNotAllowed);
    }

    return ret;
}

SrsGoApiRaw::SrsGoApiRaw(SrsServer *svr)
{
	server = svr;
}

SrsGoApiRaw::~SrsGoApiRaw()
{
}

int SrsGoApiRaw::serve_http(ISrsHttpResponseWriter* w, ISrsHttpMessage* r)
{		
    int ret = ERROR_SUCCESS;

    std::string rpc = r->query_get("rpc");

    //std::stringstream ss;

	// api/v1/raw?rpc=update&scope=listen&value=1935,1936

    // the object to return for request.
    SrsJsonObject* obj = SrsJsonAny::object();
    SrsAutoFree(SrsJsonObject, obj);
    obj->set("code", SrsJsonAny::integer(ERROR_SUCCESS));

    if (rpc == "raw") {
        return srs_api_response_code(w, r, ret);
    }
    
    //////////////////////////////////////////////////////////////////////////
    // the rpc is required.
    // the allowd rpc method check.
    if (rpc.empty() || (rpc != "reload" && rpc != "raw" && rpc != "update")) {
        ret = ERROR_SYSTEM_CONFIG_RAW;
        srs_error("raw api invalid rpc=%s. ret=%d", rpc.c_str(), ret);
        return srs_api_response_code(w, r, ret);
    }

	if (rpc == "reload") {
		srs_trace("raw api trigger reload. ret=%d", ret);
        server->on_signal(SIGNAL_RELOAD);
        return srs_api_response_code(w, r, ret);
    }
    
    // for rpc=update, to update the configs of server.
    //      @scope the scope to update for config.
    //      @value the updated value for scope.
    //      @param the extra param for scope.
    //      @data the extra data for scope.
    // possible updates:
    //      @scope          @value              value-description
    //      listen          1935,1936           the port list.
    //      pid             ./objs/srs.pid      the pid file of srs.
    //      chunk_size      60000               the global RTMP chunk_size.
    //      ff_log_dir      ./objs              the dir for ffmpeg log.
    //      srs_log_tank    file                the tank to log, file or console.
    //      srs_log_level   trace               the level of log, verbose, info, trace, warn, error.
    //      srs_log_file    ./objs/srs.log      the log file when tank is file.
    //      max_connections 1000                the max connections of srs.
    //      utc_time        false               whether enable utc time.
    //      pithy_print_ms  10000               the pithy print interval in ms.
    // vhost specified updates:
    //      @scope          @value              @param              @data               description
    //      vhost           ossrs.net           create              -                   create vhost ossrs.net
    //      vhost           ossrs.net           update              new.ossrs.net       the new name to update vhost
    // dvr specified updates:
    //      @scope          @value              @param              @data               description
    //      dvr             ossrs.net           enable              live/livestream     enable the dvr of stream
    //      dvr             ossrs.net           disable             live/livestream     disable the dvr of stream
    if (rpc == "update") {
		std::string scope = r->query_get("scope");
        std::string value = r->query_get("value");

		if (scope.empty()) {
			ret = ERROR_SYSTEM_CONFIG_RAW_NOT_ALLOWED;
            srs_error("raw api query invalid empty scope. ret=%d", ret);
            return srs_api_response_code(w, r, ret);
		}

		if (value.empty()) {
			ret = ERROR_SYSTEM_CONFIG_RAW_NOT_ALLOWED;
			srs_error("raw api query invalid empty value. ret=%d", ret);
			return srs_api_response_code(w, r, ret);
		}

        bool applied = false;
        string extra = "";
		
		if (scope == "listen") {
			vector<string> eps = srs_string_split(value, ",");

			bool invalid = eps.empty();
            for (int i = 0; i < (int)eps.size(); i++) {
                string ep = eps.at(i);
                int port = ::atoi(ep.c_str());
                if (port <= 2 || port >= 65535) {
                    invalid = true;
                    break;
                }
            }
            if (invalid) {
                ret = ERROR_SYSTEM_CONFIG_RAW_PARAMS;
                srs_error("raw api update check listen=%s failed. ret=%d", value.c_str(), ret);
                return srs_api_response_code(w, r, ret);
            }
            
            if ((ret = _srs_config->raw_set_listen(eps, applied)) != ERROR_SUCCESS) {
                srs_error("raw api update listen=%s failed. ret=%d", value.c_str(), ret);
                return srs_api_response_code(w, r, ret);
            }
        } else if (scope == "pid") {
            if (value.empty() || !srs_string_starts_with(value, "./", "/tmp/", "/var/") || !srs_string_ends_with(value, ".pid")) {
                ret = ERROR_SYSTEM_CONFIG_RAW_PARAMS;
                srs_error("raw api update check pid=%s failed. ret=%d", value.c_str(), ret);
                return srs_api_response_code(w, r, ret);
            }
            
            if ((ret = _srs_config->raw_set_pid(value, applied)) != ERROR_SUCCESS) {
                srs_error("raw api update pid=%s failed. ret=%d", value.c_str(), ret);
                return srs_api_response_code(w, r, ret);
            }
        } else if (scope == "chunk_size") {
            int csv = ::atoi(value.c_str());
            if (csv < 128 || csv > 65535 || !srs_is_digit_number(value)) {
                ret = ERROR_SYSTEM_CONFIG_RAW_PARAMS;
                srs_error("raw api update check chunk_size=%s/%d failed. ret=%d", value.c_str(), csv, ret);
                return srs_api_response_code(w, r, ret);
            }
            
            if ((ret = _srs_config->raw_set_chunk_size(value, applied)) != ERROR_SUCCESS) {
                srs_error("raw api update chunk_size=%s/%d failed. ret=%d", value.c_str(), csv, ret);
                return srs_api_response_code(w, r, ret);
            }
        } else if (scope == "ff_log_dir") {
            if (value.empty() || (value != "/dev/null" && !srs_string_starts_with(value, "./", "/tmp/", "/var/"))) {
                ret = ERROR_SYSTEM_CONFIG_RAW_PARAMS;
                srs_error("raw api update check ff_log_dir=%s failed. ret=%d", value.c_str(), ret);
                return srs_api_response_code(w, r, ret);
            }
            
            if ((ret = _srs_config->raw_set_ff_log_dir(value, applied)) != ERROR_SUCCESS) {
                srs_error("raw api update ff_log_dir=%s failed. ret=%d", value.c_str(), ret);
                return srs_api_response_code(w, r, ret);
            }
        } else if (scope == "srs_log_tank") {
            if (value.empty() || (value != "file" && value != "console")) {
                ret = ERROR_SYSTEM_CONFIG_RAW_PARAMS;
                srs_error("raw api update check srs_log_tank=%s failed. ret=%d", value.c_str(), ret);
                return srs_api_response_code(w, r, ret);
            }
            
            if ((ret = _srs_config->raw_set_srs_log_tank(value, applied)) != ERROR_SUCCESS) {
                srs_error("raw api update srs_log_tank=%s failed. ret=%d", value.c_str(), ret);
                return srs_api_response_code(w, r, ret);
            }
        } else if (scope == "srs_log_level") {
            if (value != "verbose" && value != "info" && value != "trace" && value != "warn" && value != "error") {
                ret = ERROR_SYSTEM_CONFIG_RAW_PARAMS;
                srs_error("raw api update check srs_log_level=%s failed. ret=%d", value.c_str(), ret);
                return srs_api_response_code(w, r, ret);
            }
            
            if ((ret = _srs_config->raw_set_srs_log_level(value, applied)) != ERROR_SUCCESS) {
                srs_error("raw api update srs_log_level=%s failed. ret=%d", value.c_str(), ret);
                return srs_api_response_code(w, r, ret);
            }
        } else if (scope == "srs_log_file") {
            if (value.empty() || !srs_string_starts_with(value, "./", "/tmp/", "/var/") || !srs_string_ends_with(value, ".log")) {
                ret = ERROR_SYSTEM_CONFIG_RAW_PARAMS;
                srs_error("raw api update check srs_log_file=%s failed. ret=%d", value.c_str(), ret);
                return srs_api_response_code(w, r, ret);
            }
            
            if ((ret = _srs_config->raw_set_srs_log_file(value, applied)) != ERROR_SUCCESS) {
                srs_error("raw api update srs_log_file=%s failed. ret=%d", value.c_str(), ret);
                return srs_api_response_code(w, r, ret);
            }
        } else if (scope == "max_connections") {
            int mcv = ::atoi(value.c_str());
            if (mcv < 10 || mcv > 65535 || !srs_is_digit_number(value)) {
                ret = ERROR_SYSTEM_CONFIG_RAW_PARAMS;
                srs_error("raw api update check max_connections=%s/%d failed. ret=%d", value.c_str(), mcv, ret);
                return srs_api_response_code(w, r, ret);
            }
            
            if ((ret = _srs_config->raw_set_max_connections(value, applied)) != ERROR_SUCCESS) {
                srs_error("raw api update max_connections=%s/%d failed. ret=%d", value.c_str(), mcv, ret);
                return srs_api_response_code(w, r, ret);
            }
        } else if (scope == "utc_time") {
            if (!srs_is_boolean(value)) {
                ret = ERROR_SYSTEM_CONFIG_RAW_PARAMS;
                srs_error("raw api update check utc_time=%s failed. ret=%d", value.c_str(), ret);
                return srs_api_response_code(w, r, ret);
            }
            
            if ((ret = _srs_config->raw_set_utc_time(srs_config_bool2switch(value), applied)) != ERROR_SUCCESS) {
                srs_error("raw api update utc_time=%s failed. ret=%d", value.c_str(), ret);
                return srs_api_response_code(w, r, ret);
            }
        } else if (scope == "pithy_print_ms") {
            int ppmv = ::atoi(value.c_str());
            if (ppmv < 100 || ppmv > 300000 || !srs_is_digit_number(value)) {
                ret = ERROR_SYSTEM_CONFIG_RAW_PARAMS;
                srs_error("raw api update check pithy_print_ms=%s/%d failed. ret=%d", value.c_str(), ppmv, ret);
                return srs_api_response_code(w, r, ret);
            }
            
            if ((ret = _srs_config->raw_set_pithy_print_ms(value, applied)) != ERROR_SUCCESS) {
                srs_error("raw api update pithy_print_ms=%s/%d failed. ret=%d", value.c_str(), ppmv, ret);
                return srs_api_response_code(w, r, ret);
            }
        } else if (scope == "vhost") {
            std::string param = r->query_get("param");
            std::string data = r->query_get("data");
            if (param != "create" && param != "update" && param != "delete" && param != "disable" && param != "enable") {
                ret = ERROR_SYSTEM_CONFIG_RAW_NOT_ALLOWED;
                srs_error("raw api query invalid scope=%s, param=%s. ret=%d", scope.c_str(), param.c_str(), ret);
                return srs_api_response_code(w, r, ret);
            }
            extra += " " + param;
            
            if (param == "create") {
                // when create, the vhost must not exists.
                if (param.empty() || _srs_config->get_vhost(value, false)) {
                    ret = ERROR_SYSTEM_CONFIG_RAW_PARAMS;
                    srs_error("raw api update check vhost=%s, param=%s failed. ret=%d", value.c_str(), param.c_str(), ret);
                    return srs_api_response_code(w, r, ret);
                }

                string body;
                if ((ret = r->body_read_all(body)) != ERROR_SUCCESS) {
                    srs_error("raw api update vhost=%s read body failed, ret=%d", value.c_str(), ret);
                    return ret;
                }
                
                if ((ret = _srs_config->raw_create_vhost(value, body, applied)) != ERROR_SUCCESS) {
                    srs_error("raw api update vhost=%s, param=%s failed. ret=%d", value.c_str(), param.c_str(), ret);
                    return srs_api_response_code(w, r, ret);
                }
            } else if (param == "update") {
                extra += " to " + data;
                
                // when update, the vhost must exists and disabled.
                SrsConfDirective* vhost = _srs_config->get_vhost(value, false);
                if (data.empty() || data == value || param.empty() || !vhost || _srs_config->get_vhost_enabled(vhost)) {
                    ret = ERROR_SYSTEM_CONFIG_RAW_PARAMS;
                    srs_error("raw api update check vhost=%s, param=%s, data=%s failed. ret=%d", value.c_str(), param.c_str(), data.c_str(), ret);
                    return srs_api_response_code(w, r, ret);
                }
                
                if ((ret = _srs_config->raw_update_vhost(value, data, applied)) != ERROR_SUCCESS) {
                    srs_error("raw api update vhost=%s, param=%s, data=%s failed. ret=%d", value.c_str(), param.c_str(), data.c_str(), ret);
                    return srs_api_response_code(w, r, ret);
                }
            } else if (param == "delete") {
                // when delete, the vhost must exists and disabled.
                SrsConfDirective* vhost = _srs_config->get_vhost(value, false);
                if (param.empty() || !vhost || _srs_config->get_vhost_enabled(vhost)) {
                    ret = ERROR_SYSTEM_CONFIG_RAW_PARAMS;
                    srs_error("raw api update check vhost=%s, param=%s failed. ret=%d", value.c_str(), param.c_str(), ret);
                    return srs_api_response_code(w, r, ret);
                }
                
                if ((ret = _srs_config->raw_delete_vhost(value, applied)) != ERROR_SUCCESS) {
                    srs_error("raw api update vhost=%s, param=%s failed. ret=%d", value.c_str(), param.c_str(), ret);
                    return srs_api_response_code(w, r, ret);
                }
            } else if (param == "disable") {
                // when disable, the vhost must exists and enabled.
                SrsConfDirective* vhost = _srs_config->get_vhost(value, false);
                if (param.empty() || !vhost || !_srs_config->get_vhost_enabled(vhost)) {
                    ret = ERROR_SYSTEM_CONFIG_RAW_PARAMS;
                    srs_error("raw api update check vhost=%s, param=%s failed. ret=%d", value.c_str(), param.c_str(), ret);
                    return srs_api_response_code(w, r, ret);
                }
                
                if ((ret = _srs_config->raw_disable_vhost(value, applied)) != ERROR_SUCCESS) {
                    srs_error("raw api update vhost=%s, param=%s failed. ret=%d", value.c_str(), param.c_str(), ret);
                    return srs_api_response_code(w, r, ret);
                }
            } else if (param == "enable") {
                // when enable, the vhost must exists and disabled.
                SrsConfDirective* vhost = _srs_config->get_vhost(value, false);
                if (param.empty() || !vhost || _srs_config->get_vhost_enabled(vhost)) {
                    ret = ERROR_SYSTEM_CONFIG_RAW_PARAMS;
                    srs_error("raw api update check vhost=%s, param=%s failed. ret=%d", value.c_str(), param.c_str(), ret);
                    return srs_api_response_code(w, r, ret);
                }
                
                if ((ret = _srs_config->raw_enable_vhost(value, applied)) != ERROR_SUCCESS) {
                    srs_error("raw api update vhost=%s, param=%s failed. ret=%d", value.c_str(), param.c_str(), ret);
                    return srs_api_response_code(w, r, ret);
                }
            } else {
                // TODO: support other param.
            }
        } else if (scope == "dvr") {
            std::string action = r->query_get("param");
            std::string stream = r->query_get("data");
            extra += "/" + stream + " to " + action;
            
            if (action != "enable" && action != "disable") {
                ret = ERROR_SYSTEM_CONFIG_RAW_NOT_ALLOWED;
                srs_error("raw api query invalid scope=%s, param=%s. ret=%d", scope.c_str(), action.c_str(), ret);
                return srs_api_response_code(w, r, ret);
            }
            
            if (!_srs_config->get_dvr_enabled(value)) {
                ret = ERROR_SYSTEM_CONFIG_RAW_NOT_ALLOWED;
                srs_error("raw api query invalid scope=%s, value=%s, param=%s. ret=%d", scope.c_str(), value.c_str(), action.c_str(), ret);
                return srs_api_response_code(w, r, ret);
            }
            
            if (action == "enable") {
                if ((ret = _srs_config->raw_enable_dvr(value, stream, applied)) != ERROR_SUCCESS) {
                    srs_error("raw api update dvr=%s/%s, param=%s failed. ret=%d", value.c_str(), stream.c_str(), action.c_str(), ret);
                    return srs_api_response_code(w, r, ret);
                }
            } else {
                if ((ret = _srs_config->raw_disable_dvr(value, stream, applied)) != ERROR_SUCCESS) {
                    srs_error("raw api update dvr=%s/%s, param=%s failed. ret=%d", value.c_str(), stream.c_str(), action.c_str(), ret);
                    return srs_api_response_code(w, r, ret);
                }
            }
        } else {
            // TODO: support other scope.
        }
        
        // whether the config applied.
        if (applied) {
            server->on_signal(SIGNAL_WRITE_CONFIG);
			//server->on_signal(SIGNAL_RELOAD);
            srs_trace("raw api update %s=%s%s ok.", scope.c_str(), value.c_str(), extra.c_str());
        } else {
            srs_warn("raw api update not applied %s=%s%s.", scope.c_str(), value.c_str(), extra.c_str());
        }
        
        return srs_api_response(w, r, obj->dumps());		
	}

	
	return srs_api_response_code(w, r, ret);
//    return ret;
}

SrsGoApiError::SrsGoApiError()
{
}

SrsGoApiError::~SrsGoApiError()
{
}

int SrsGoApiError::serve_http(ISrsHttpResponseWriter* w, ISrsHttpMessage* r)
{
    return srs_api_response_code(w, r, 100);
}


SrsHttpApi::SrsHttpApi(IConnectionManager* cm, st_netfd_t fd, SrsHttpServeMux* m)
    : SrsConnection(cm, fd)
{
    mux = m;
    parser = new SrsHttpParser();
    crossdomain_required = false;
}

SrsHttpApi::~SrsHttpApi()
{
    srs_freep(parser);
}

void SrsHttpApi::resample()
{
    // TODO: FIXME: implements it
}

int64_t SrsHttpApi::get_send_bytes_delta()
{
    // TODO: FIXME: implements it
    return 0;
}

int64_t SrsHttpApi::get_recv_bytes_delta()
{
    // TODO: FIXME: implements it
    return 0;
}

void SrsHttpApi::cleanup()
{
    // TODO: FIXME: implements it
}

int SrsHttpApi::do_cycle()
{
    int ret = ERROR_SUCCESS;
    
    srs_trace("api get peer ip success. ip=%s", ip.c_str());
    
    // initialize parser
    if ((ret = parser->initialize(HTTP_REQUEST)) != ERROR_SUCCESS) {
        srs_error("api initialize http parser failed. ret=%d", ret);
        return ret;
    }
    
    // underlayer socket
    SrsStSocket skt(stfd);
    
    // set the recv timeout, for some clients never disconnect the connection.
    // @see https://github.com/ossrs/srs/issues/398
    skt.set_recv_timeout(SRS_HTTP_RECV_TIMEOUT_US);
    
    // process http messages.
    while(!disposed) {
        ISrsHttpMessage* req = NULL;
        
        // get a http message
        if ((ret = parser->parse_message(&skt, this, &req)) != ERROR_SUCCESS) {
            return ret;
        }

        // if SUCCESS, always NOT-NULL.
        srs_assert(req);
        
        // always free it in this scope.
        SrsAutoFree(ISrsHttpMessage, req);
        
        // ok, handle http request.
        SrsHttpResponseWriter writer(&skt);
        if ((ret = process_request(&writer, req)) != ERROR_SUCCESS) {
            return ret;
        }

        // read all rest bytes in request body.
        char buf[SRS_HTTP_READ_CACHE_BYTES];
        ISrsHttpResponseReader* br = req->body_reader();
        while (!br->eof()) {
            if ((ret = br->read(buf, SRS_HTTP_READ_CACHE_BYTES, NULL)) != ERROR_SUCCESS) {
                return ret;
            }
        }

        // donot keep alive, disconnect it.
        // @see https://github.com/ossrs/srs/issues/399
        if (!req->is_keep_alive()) {
            break;
        }
    }
        
    return ret;
}

int SrsHttpApi::process_request(ISrsHttpResponseWriter* w, ISrsHttpMessage* r) 
{
    int ret = ERROR_SUCCESS;
    
    SrsHttpMessage* hm = dynamic_cast<SrsHttpMessage*>(r);
    srs_assert(hm);
    
    srs_trace("HTTP API %s %s, content-length=%"PRId64", chunked=%d/%d",
        r->method_str().c_str(), r->url().c_str(), r->content_length(),
        hm->is_chunked(), hm->is_infinite_chunked());
    
    // method is OPTIONS and enable crossdomain, required crossdomain header.
    if (r->is_http_options() && _srs_config->get_http_api_crossdomain()) {
        crossdomain_required = true;
    }

    // whenever crossdomain required, set crossdomain header.
    if (crossdomain_required) {
        w->header()->set("Access-Control-Allow-Origin", "*");
        w->header()->set("Access-Control-Allow-Methods", "GET, POST, HEAD, PUT, DELETE");
        w->header()->set("Access-Control-Allow-Headers", "Cache-Control,X-Proxy-Authorization,X-Requested-With,Content-Type");
    }

    // handle the http options.
    if (r->is_http_options()) {
        w->header()->set_content_length(0);
        if (_srs_config->get_http_api_crossdomain()) {
            w->write_header(SRS_CONSTS_HTTP_OK);
        } else {
            w->write_header(SRS_CONSTS_HTTP_MethodNotAllowed);
        }
        return w->final_request();
    }
    
    // use default server mux to serve http request.
    if ((ret = mux->serve_http(w, r)) != ERROR_SUCCESS) {
        if (!srs_is_client_gracefully_close(ret)) {
            srs_error("serve http msg failed. ret=%d", ret);
        }
        return ret;
    }
    
    return ret;
}

#endif

