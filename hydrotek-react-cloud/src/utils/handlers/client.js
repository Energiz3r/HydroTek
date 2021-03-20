import io from 'socket.io-client';
import { buildForDev } from '../../config';

export let socket;
export let socketEnabled = false;
export const enableSocket = () => {
  if (buildForDev) {
    socket = io('http://localhost:3000');
  } else {
    socket = io();
  }
  setTimeout(()=>{socketEnabled = true}, 50)
}
