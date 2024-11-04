#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

typedef struct s_register {
    bool pass;
    bool nick;
    bool user;
    bool registered;
} t_regitser;

class Client {
	private:
		int			_fd;
		std::string _msgBuf;
		std::string	_nickname;
		std::string	_username;
		std::string	_hostname;
		std::string	_realname;
		std::string _password;
		std::string _servername;
		time_t		_lastPingTime;
		time_t		_lastPongTime;
		t_regitser	_registerStatus;


	public:
		Client();
		Client(const std::string& str);
		~Client();

		void		setNickname(const std::string& str);
		void		setUsername(const std::string& str);
		void		setHostname(const std::string& str);
		void		setRealname(const std::string& str);
		void		setPassword(const std::string str);
		void		setServername(const std::string& str); //단일 서버라도 서버이름은 필요함
		void 		setLastPongTime();
		void		setIsRegistered(bool flag);
		void		setPassStatus(bool status);
		void 		setNickStatus(bool status);
		void 		setUserStatus(bool status);
		void 		setRegisteredStatus(bool status);

		int			getFd() const;
		std::string	getNickname() const;
		std::string	getUsername() const;
		std::string	getHostname() const;
		std::string	getRealname() const;
		std::string	getPassword() const;
		std::string	getServername() const;
		time_t		getLastPongTime() const;
		bool		getPassStatus() const;
		bool		getNickStatus() const;
		bool		getUserStatus() const;
		bool		getRegisteredStatus() const;

		void		appendToBuffer(const std::string& str);
		bool		extractMessage(std::string& message);
		bool		isConnectionTimedOut(time_t timeout);
};

#endif
