#ifndef _DOWNLOAD_H_
#define _DOWNLOAD_H_

#include <string>
#include <memory>

class Response
{
public:
	Response() : resultCode(0), valid(true), msg(), fname() {}
	int resultCode;
	bool valid;
	std::string msg;
	std::string fname;
};

class DownloadAgent
{
public:
	virtual ~DownloadAgent() {}

	/** set the http user agent reported to the server */
	virtual void setUserAgent(const std::string &agent) = 0;

	/** thread safe. Call this to interrupt a download */
	virtual void interrupt() = 0;


	/**
	 * download file from url.
	 * use temp file, remove file if download failed
	 * Long running operation, call from a worker thread to avoid blocking UI.
	 */
	virtual Response downloadToFile(const std::string &url, const std::string &fname) = 0;

	static std::shared_ptr<DownloadAgent> newInstance();
};


#endif
