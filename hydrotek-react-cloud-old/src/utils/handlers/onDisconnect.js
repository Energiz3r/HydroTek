import { store } from '../../stores/store';
import {
  setPing,
  setDisconnected,
  setDisconnectionReason,
  setWaitingForNickAcceptance,
  setAllChannelsWasJoined
} from '../../actions/actions';

//
// handle disconnections and errors
//

const handleDisconnect = (reason) => {
  store.dispatch(setPing('--'));
  store.dispatch(setDisconnected());
  if (reason == 'io server disconnect') {
    reason = "Server terminated connection, will not attempt to re-establish."
  }
  store.dispatch(setDisconnectionReason(reason));
  store.dispatch(setAllChannelsWasJoined());
}

export default handleDisconnect;