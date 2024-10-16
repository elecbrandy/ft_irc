#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {
private:
	std::string	_nickname;
	std::string	_username;
	std::string	_hostname;
	std::string	_realname;
	std::string _password;
	std::string _servername; 
public:
	Client();
	Client(const std::string& str);
	~Client();

	void		setNickname(const std::string& str);
	void		setUsername(const std::string& str);
	void		setHostname(const std::string& str);
	void		setRealname(const std::string& str);
	void		setPassword(const std::string& str);
	void		setServername(const std::string& str); //단일 서버라도 서버이름은 필요함

	std::string	getNickname() const;
	std::string	getUsername() const;
	std::string	getHostname() const;
	std::string	getRealname() const;
	std::string	getPassword() const;
	std::string	getServername() const;
};

#endif
