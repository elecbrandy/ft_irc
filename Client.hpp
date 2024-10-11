#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {
private:
	std::string	name;

public:
	Client();
	Client(const std::string& str);
	~Client();

	std::string	getName() const;
	void		setName(const std::string& str);
};

#endif
