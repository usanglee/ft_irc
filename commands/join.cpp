#include "command.hpp"

void join(User &user, Channel &channel, vector<string> command, int fd){

    string target = command.size() > 1 ? command[1] : "";
    string nickName = user.getNickName(fd);
    string loginName = user.getLoginName(fd);
    string hostName = user.getHostName(fd);
    
    if (target == "" || (target.size() == 1 && target[0] == '#')) {
        user.setWriteBuffer(fd, serverMessage(ERR_NEEDMOREPARAMS, nickName, "", "", "Not enough parameters"));
        return;
    }

    if (target[0] != '#') {
        user.setWriteBuffer(fd, serverMessage(ERR_BADCHANMASK, nickName, target, "", "Bad Channel Mask"));
        return;
    }

    vector<string> channelNames = split(target, ","); // 채널이 a,b,c,d로 들어온 경우 처리

    for (size_t i = 0; i < channelNames.size(); ++i)
    {
        string channelName = i == 0 ? channelNames[i] : "#" + channelNames[i];
        string message = prefixMessage(nickName, loginName, hostName, "JOIN", channelName);

        if (!channel.isValidChannel(channelName))
            channel.addChannel(channelName);

        if (channel.hasUser(channelName, fd)) // 채널에 이미 가입해 있는 경우는 거름
            continue;
        

        channel.addUser(channelName, fd);
        user.addChannel(fd, channelName);
        
        channel.setBroadCastMessage(channelName, 0, message, user);
        user.setWriteBuffer(fd, serverMessage(RPL_NAMREPLY, nickName, loginName, channelName, channel.getUserList(user, channelName, fd)));
        user.setWriteBuffer(fd, serverMessage(RPL_ENDOFNAMES, nickName, loginName, channelName, "End of NAMES list"));
    }
}
