import { store } from '../../stores/store';
import requestJoinChannel from './requestJoinChannel';
import { setAppReady } from '../setAppState';
import { setNick, setAdmin, setLoggedIn } from '../../actions/actions';
import log from '../log'

// handle session restoration
const onLoginRestore = ({isAdmin, nick, channels, loggedIn}) => {  

  log("Existing session found, restoring info for '" + nick + "'." + isAdmin)

  store.dispatch(setNick(nick));

  if (loggedIn) {
    store.dispatch(setLoggedIn());
    //join last known channels the user was in
    if (channels.map) {
      channels.map((channel)=>{
        requestJoinChannel(channel.id)
      })
    }
    //go to the app
    setAppReady();
  }

  if (isAdmin) {
    store.dispatch(setAdmin());
  }  

}

export default onLoginRestore;