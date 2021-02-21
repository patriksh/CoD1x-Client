#include "curl.hpp"
#include "functions.hpp"

extern char* (*qcurl_version)(void);
CURL* (*qcurl_easy_init)(void);
CURLcode(*qcurl_easy_setopt)(CURL* curl, CURLoption option, ...);
CURLcode(*qcurl_easy_perform)(CURL* curl);
void (*qcurl_easy_cleanup)(CURL* curl);
CURLcode(*qcurl_easy_getinfo)(CURL* curl, CURLINFO info, ...);
void (*qcurl_easy_reset)(CURL* curl);
const char* (*qcurl_easy_strerror)(CURLcode);
CURLM* (*qcurl_multi_init)(void);
CURLMcode(*qcurl_multi_add_handle)(CURLM* multi_handle, CURL* curl_handle);
CURLMcode(*qcurl_multi_remove_handle)(CURLM* multi_handle, CURL* curl_handle);
CURLMcode(*qcurl_multi_fdset)(CURLM* multi_handle, fd_set* read_fd_set, fd_set* write_fd_set, fd_set* exc_fd_set, int* max_fd);
CURLMcode(*qcurl_multi_perform)(CURLM* multi_handle, int* running_handles);
CURLMcode(*qcurl_multi_cleanup)(CURLM* multi_handle);
CURLMsg* (*qcurl_multi_info_read)(CURLM* multi_handle, int* msgs_in_queue);
const char* (*qcurl_multi_strerror)(CURLMcode);

qboolean cURLUsed;
qboolean downloadListSet;
char baseURL[MAX_OSPATH];
char downloadURL[MAX_OSPATH];
CURL* downloadCURL;
CURLM* downloadCURLM;
fileHandle_t download;
char downloadTempName[MAX_OSPATH];
char downloadName[MAX_OSPATH];
int downloadBlock;
int downloadCount;
int downloadSize;
char downloadList[MAX_INFO_STRING];

#define cls_realtime ((int*)0x155F3E0)


void CL_cURL_Cleanup(void) {
	if (downloadCURLM) {
		CURLMcode result;

		if (downloadCURL) {
			result = curl_multi_remove_handle(downloadCURLM, downloadCURL);
			if (result != CURLM_OK) {
				Com_Printf("qcurl_multi_remove_handle failed: %s\n", curl_multi_strerror(result));
			}
			curl_easy_cleanup(downloadCURL);
		}
		result = curl_multi_cleanup(downloadCURLM);
		if (result != CURLM_OK) {
			Com_Printf("CL_cURL_Cleanup: qcurl_multi_cleanup failed: %s\n", curl_multi_strerror(result));
		}
		downloadCURLM = NULL;
		downloadCURL = NULL;
	}
	else if (downloadCURL) {
		curl_easy_cleanup(downloadCURL);
		downloadCURL = NULL;
	}
}

static int CL_cURL_CallbackProgress(void* dummy, double dltotal, double dlnow, double ultotal, double ulnow) {
	downloadSize = (int)dltotal;
	Cvar_SetValue("cl_downloadSize", downloadSize);
	downloadCount = (int)dlnow;
	Cvar_SetValue("cl_downloadCount", downloadCount);
	return 0;
}

static size_t CL_cURL_CallbackWrite(void* buffer, size_t size, size_t nmemb, void* stream) {
	FS_Write(buffer, size * nmemb, ((fileHandle_t*)stream)[0]);
	return size * nmemb;
}

CURLcode qcurl_easy_setopt_warn(CURL* curl, CURLoption option, ...) {
	CURLcode result;

	va_list argp;
	va_start(argp, option);

	if (option < CURLOPTTYPE_OBJECTPOINT) {
		long longValue = va_arg(argp, long);
		result = curl_easy_setopt(curl, option, longValue);
	}
	else if (option < CURLOPTTYPE_OFF_T) {
		void* pointerValue = va_arg(argp, void*);
		result = curl_easy_setopt(curl, option, pointerValue);
	}
	else {
		curl_off_t offsetValue = va_arg(argp, curl_off_t);
		result = curl_easy_setopt(curl, option, offsetValue);
	}

	if (result != CURLE_OK) {
		Com_Printf("qcurl_easy_setopt failed: %s\n", curl_easy_strerror(result));
	}
	va_end(argp);

	return result;
}

void X_CL_DownloadsComplete(void) {
	if (cURLUsed) {
		cURLUsed = qfalse;
		downloadListSet = qfalse;
		CL_cURL_Cleanup();
		FS_Restart(0);
	}

	CL_DownloadsComplete();
}

void CL_cURL_BeginDownload(const char* localName, const char* remoteURL) {
	CURLMcode result;
	cURLUsed = qtrue;

	CL_cURL_Cleanup();
	Com_Sprintf(downloadURL, sizeof(downloadURL), "%s/%s", baseURL, remoteURL);
	strncpy(downloadName, localName, sizeof(downloadName));
	Com_Sprintf(downloadTempName, sizeof(downloadTempName), "%s.tmp", localName);

	Cvar_Set("cl_downloadName", downloadName);
	Cvar_Set("cl_downloadSize", "0");
	Cvar_Set("cl_downloadCount", "0");
	Cvar_Set("cl_downloadTime", va("%i", *cls_realtime));

	downloadBlock = 0;
	downloadCount = 0;

	downloadCURL = curl_easy_init();
	if (!downloadCURL) {
		Com_Error(ERR_DROP, "CL_cURL_BeginDownload: qcurl_easy_init() failed");
		return;
	}
	download = FS_SV_FOpenFileWrite(downloadTempName);
	if (!download) {
		Com_Error(ERR_DROP, "CL_cURL_BeginDownload: failed to open %s for writing", downloadTempName);
		return;
	}

	qcurl_easy_setopt_warn(downloadCURL, CURLOPT_URL, downloadURL);
	qcurl_easy_setopt_warn(downloadCURL, CURLOPT_TRANSFERTEXT, 0);
	qcurl_easy_setopt_warn(downloadCURL, CURLOPT_WRITEFUNCTION, CL_cURL_CallbackWrite);
	qcurl_easy_setopt_warn(downloadCURL, CURLOPT_WRITEDATA, &download);
	qcurl_easy_setopt_warn(downloadCURL, CURLOPT_NOPROGRESS, 0);
	qcurl_easy_setopt_warn(downloadCURL, CURLOPT_PROGRESSFUNCTION, CL_cURL_CallbackProgress);
	qcurl_easy_setopt_warn(downloadCURL, CURLOPT_PROGRESSDATA, NULL);
	qcurl_easy_setopt_warn(downloadCURL, CURLOPT_FAILONERROR, 1);
	qcurl_easy_setopt_warn(downloadCURL, CURLOPT_FOLLOWLOCATION, 1);
	qcurl_easy_setopt_warn(downloadCURL, CURLOPT_MAXREDIRS, 5);
	qcurl_easy_setopt_warn(downloadCURL, CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS | CURLPROTO_FTP | CURLPROTO_FTPS);
	qcurl_easy_setopt_warn(downloadCURL, CURLOPT_BUFFERSIZE, CURL_MAX_READ_SIZE);
	downloadCURLM = curl_multi_init();
	if (!downloadCURLM) {
		curl_easy_cleanup(downloadCURL);
		downloadCURL = NULL;
		Com_Error(ERR_DROP, "CL_cURL_BeginDownload: qcurl_multi_init() failed");
		return;
	}
	result = curl_multi_add_handle(downloadCURLM, downloadCURL);
	if (result != CURLM_OK) {
		curl_easy_cleanup(downloadCURL);
		downloadCURL = NULL;
		Com_Error(ERR_DROP, "CL_cURL_BeginDownload: qcurl_multi_add_handle() failed: %s", curl_multi_strerror(result));
		return;
	}
}

void X_CL_cURL_NextDownload(void) {
	Cvar_VariableStringBuffer("sv_wwwBaseURL", baseURL, sizeof(baseURL));

	char* s;
	char* remoteName, * localName;
	if (!downloadListSet) {
		memcpy(downloadList, (char*)0x15EEBBC, sizeof(downloadList));
		downloadListSet = qtrue;
	}

	Cvar_Set("cl_downloadName", "");

	if (*downloadList) {
		s = downloadList;

		if (*s == '@')
			s++;
		remoteName = s;

		if ((s = strchr(s, '@')) == NULL) {
			X_CL_DownloadsComplete();
			return;
		}

		*s++ = 0;
		localName = s;
		if ((s = strchr(s, '@')) != NULL)
			*s++ = 0;
		else
			s = localName + strlen(localName);

		CL_cURL_BeginDownload(localName, remoteName);

		memmove(downloadList, s, strlen(s) + 1);

		return;
	}

	X_CL_DownloadsComplete();
}


void X_CL_NextDownload(void) {
	if (Cvar_VariableString("sv_wwwBaseUrl") == "") {
		CL_NextDownload();
	}
	else {
		X_CL_cURL_NextDownload();
	}
}

void CL_cURL_PerformDownload(void) {
	CURLMcode res;
	CURLMsg* msg;
	int c;
	int i = 0;

	res = curl_multi_perform(downloadCURLM, &c);
	while (res == CURLM_CALL_MULTI_PERFORM && i < 100) {
		res = curl_multi_perform(downloadCURLM, &c);
		i++;
	}
	if (res == CURLM_CALL_MULTI_PERFORM)
		return;
	msg = curl_multi_info_read(downloadCURLM, &c);
	if (msg == NULL) {
		return;
	}
	FS_FCloseFile(download);
	if (msg->msg == CURLMSG_DONE && msg->data.result == CURLE_OK) {
		FS_SV_Rename(downloadTempName, downloadName);
	}
	else {
		long code;
		curl_easy_getinfo(msg->easy_handle, CURLINFO_RESPONSE_CODE, &code);
		Com_Error(ERR_DROP, "Download Error: %s Code: %ld URL: %s", curl_easy_strerror(msg->data.result), code, downloadURL);
	}

	X_CL_NextDownload();
}

void X_Download_Frame() {
	if (downloadCURLM) {
		CL_cURL_PerformDownload();
	}
}
