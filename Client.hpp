#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <arpa/inet.h>

typedef struct s_register {
    bool pass;
    bool nick;
    bool user;
    bool registered;
} t_regitser;

class Client {
	private:
		int			_fd;
		std::string	_recvBuffer;
		std::string	_sendBuffer;
		std::string	_nickname;
		std::string	_username;
		std::string	_hostname;
		std::string	_realname;
		std::string _servername;
		time_t		_LastPingSent; // 클라이언트가 마지막으로 ping을 보낸시간
		time_t		_lastPingReceived; // 클라이언트가 서버로부터 마지막으로 ping을 받은 시간 (ping을 보내지 않아서 서버가 ping을 보내는 경우)	
		t_regitser	_registerStatus;
		std::string _prefix;

	public:
		Client(in_addr addr);
		Client(const std::string& str);
		~Client();

		void		setFd(int fd);
		void		setNickname(const std::string& str);
		void		setUsername(const std::string& str);
		void		setHostname(const std::string& str);
		void		setRealname(const std::string& str);
		// void		setPassword(const std::string str);
		void		setServername(const std::string& str);
		void 		updateLastPingSent();
		void		setIsRegistered(bool flag);
		void		setPassStatus(bool status);
		void 		setNickStatus(bool status);
		void 		setUserStatus(bool status);
		void 		setRegisteredStatus(bool status);
		void 		setPrefix();

		int			getFd() const;
		std::string	getNickname() const;
		std::string	getUsername() const;
		std::string	getHostname() const;
		std::string	getRealname() const;
		std::string	getPassword() const;
		std::string	getServername() const;
		time_t		getLastPingSent() const;
		bool		getPassStatus() const;
		bool		getNickStatus() const;
		bool		getUserStatus() const;
		bool		getRegisteredStatus() const;
		std::string getPrefix() const;

		void		appendToRecvBuffer(const std::string& str);
		bool		extractMessage(std::string& message);
		bool		isTimedOut();

		void		printLog();
		void		appendToSendBuffer(const std::string& str);
		const std::string& getSendBuffer() const;
		void		clearSendBuffer(size_t count);
		bool		hasDataToSend() const;
};

#endif
