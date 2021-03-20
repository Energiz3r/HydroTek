import { createStore, combineReducers } from 'redux'
import UIReducer from '../reducers/UIReducer'
import loginReducer from '../reducers/loginReducer'
import { buildForDev } from '../config'
import { initLoginState } from '../actions/loginActions'
import { initUIState } from '../actions/UIActions';


//CREATE STORE
const configureStore = () => {

  const reducers = {
    UI: UIReducer,
    login: loginReducer
  };
  
  if (buildForDev) {
    var store = createStore(combineReducers(reducers), window.__REDUX_DEVTOOLS_EXTENSION__ && window.__REDUX_DEVTOOLS_EXTENSION__() ); //enables the REDUX plugin to talk to the corresponding Chrome extension
  } else {
    var store = createStore(combineReducers(reducers));
  }

  return store;
  
};

export const store = configureStore();

// populate the state with default values
store.dispatch(initUIState());
store.dispatch(initLoginState());

