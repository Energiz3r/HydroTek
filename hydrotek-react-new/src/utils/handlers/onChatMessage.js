import { store } from '../../stores/store';
import {
  addMessage ,setChannelHasNotifs ,setChannelHasMessages ,setChannelHasMention ,setMessagesSinceNotFocused } from '../../actions/actions';
import getCurrentChannel from '../getCurrentChannel';
import { systemNick } from '../../config';
import log from '../log'
import { getNowTimestamp } from '../dateUtils';

//handle incoming chat messages
const onChatMessage = (msg) => {

  //use our own timestamp
  msg.receivedTimestamp = getNowTimestamp();

  store.dispatch(addMessage({...msg, messageSent: true}));
  //set notifications for messages in other channels
  log(getCurrentChannel() + ' and ' + msg.channelId)
  if (msg.channelId != getCurrentChannel()) {
    //if the message was a system message
    if (msg.source == systemNick) {
      store.dispatch(setChannelHasNotifs(msg.channelId));
    }
    //if the message was a regular message
    else {
      store.dispatch(setChannelHasMessages(msg.channelId));
    }
    //if the message mentions the user's nick in it
    if (msg.messageText.includes(store.getState().loginState.nick)) {
      store.dispatch(setChannelHasMention(msg.channelId));
    }
  }
  //if the window isn't focused and the message was a message or a mention
  if (!store.getState().userInterface.appIsFocused && msg.source != systemNick) {
    store.dispatch(setMessagesSinceNotFocused());
  }

};

export default onChatMessage;