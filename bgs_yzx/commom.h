

std::string getCurrentTime()
{
	time_t rawtime;
	time(&rawtime);
	struct tm timeinfo;
	localtime_s(&timeinfo, &rawtime);
	char buf[70];
	asctime_s(buf, &timeinfo);
	std::string res(buf);
	return buf;
}