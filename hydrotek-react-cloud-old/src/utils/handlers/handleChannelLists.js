import { store } from '../../stores/store';
import { addChannel, setDefaultChannelsReceived, setLoginModalVisible } from '../../actions/actions';
import log from '../log'

export const requestDefaultChannels = (socket) => {
  if (!store.getState().userInterface.defaultChannelsReceived) { 
    socket.emit('request default channels'); //send a request for default channel list only if we don't have one
  }
}

//handle receiving default channels from the server
export const onDefaultChannel = (channelObject) => {
  if (!store.getState().userInterface.defaultChannelsReceived) {
    const channel = {
      ...channelObject,
      channelName: channelObject.name //this field is named differently on the server side
    }
    delete channel.name //remove the differently-name field
    store.dispatch(addChannel(channel)); //only do this if channels haven't already been received
  }
};

//handle finish receiving of default channels
export const onDefaultChannelsFinished = () => {
  log("List of default channels received!")
  store.dispatch(setDefaultChannelsReceived());
  if (!store.getState().loginState.loggedIn) {
    log("Showing login screen...")
    store.dispatch(setLoginModalVisible()); //only show the modal if not already logged in
  }
};