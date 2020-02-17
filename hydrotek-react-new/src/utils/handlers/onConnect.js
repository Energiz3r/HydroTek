import { store } from '../../stores/store';
import { requestDefaultChannels } from './handleChannelLists';
import requestJoinChannel from './requestJoinChannel';
//import requestSetNick from './requestSetNick';
//import { requestUserList } from './handleUserLists';
import { setConnected } from '../../actions/actions';
import Cookie from 'js-cookie';
import log from '../log'

const cookieDomain = 'localhost'
const useSecure = false

//rejoin channels after a disconnection
const rejoinChannels = (socket) => {
  store.getState().channels.map((channel) => {
    if (channel.wasJoined) { //this flag marks channels the user was disconnected from
      requestJoinChannel(channel.channelId);
    }
  });
}

//await a login from the user
const setAwaitLogin = (socket) => {
  
  log("Downloading channel list and awaiting login.");
  
  //get the default channel list if needed
  requestDefaultChannels(socket);

}

//
// handle connections to server
//
const onConnect = (socket, wasReconnect) => {  

  log("Connected!");

  //set UI state to connected
  store.dispatch(setConnected());

  //load the session cookie if it exists and create a session
  var sessionKey = Cookie.get('sessionKey');

  if (!sessionKey) { //if there was no cookie, create new session and cookie

    log("Requesting session...");
    socket.emit('create session', (response)=>{
      var expiry = new Date(new Date().getTime() + 30 * 60 * 1000); // 30 minutes
      Cookie.set('sessionKey', response, {
        expires: expiry, 
        domain: cookieDomain,
        secure: useSecure
      });
      log("Session (new): " + response);
      setAwaitLogin(socket);
    });

  } else {

    log("Session found, checking with server... " + sessionKey)
    socket.emit('check session', sessionKey, (response)=>{
      if (response == "no session") { //if there was a cookie but the server doesn't recognise it
        socket.emit('create session', (response)=>{
          var expiry = new Date(new Date().getTime() + 30 * 60 * 1000); // 30 minutes
          Cookie.set('sessionKey', response, {
            expires: expiry, 
            domain: cookieDomain,
            secure: useSecure
          });
          log("Session rejected, new one was created.");
          setAwaitLogin(socket);

        });

      } else if (response == "success") { //if there was a cookie and the server recognises it
        log("Session accepted!");
        setAwaitLogin(socket);
      }

      else {
        log("Session error: " + response);
      }
    });
  }

}

export default onConnect;