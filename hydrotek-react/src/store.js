import { createStore, combineReducers } from 'redux'
import uiReducer from './reducers/uiReducer'

//create redux store
 const configureStore = () => {
  const configstore = createStore(
    combineReducers({
      ui: uiReducer
    }), window.__REDUX_DEVTOOLS_EXTENSION__ && window.__REDUX_DEVTOOLS_EXTENSION__()
  );
  return configstore;
};

export default configureStore()
