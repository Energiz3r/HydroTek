import requestJoinChannel from './requestJoinChannel'
import { addChannel } from '../../actions/actions';
import { store } from '../../stores/store';

//request to join default channels
export default () => {
  store.getState().channels.map((channel) => {
    if (channel.isSelectedInPicker && channel.isDefault) {
      const joinedChannel = store.getState().channels.filter(getchannel => getchannel.channelId == channel.channelId)[0];
      if (!joinedChannel || !joinedChannel.isJoined) { //do a check first to make sure the channel isn't already joined
        // here, send the actual server request to join the channel
        requestJoinChannel(channel.channelId)
      }
    }
  });
}