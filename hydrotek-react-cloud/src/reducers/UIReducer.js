const reducerDefaultState = []

export default (state = reducerDefaultState, action) => {
  switch (action.type) {
    case 'INIT_UI_STATE':
      return action.UI
    case 'BLUR_APP':
      return {
        ...state,
        appIsBlurred: true
      }
    case 'UNBLUR_APP':
      return {
        ...state,
        appIsBlurred: false
      }
    case 'SET_ROUTE':
      return {
        ...state,
        route: action.route
      }
    case 'SET_INITIAL_ROUTE':
      return {
        ...state,
        initialRoute: action.route,
        initialRouteRedirect: action.route != ''
      }
    case 'UNSET_INITIAL_ROUTE':
      return {
        ...state,
        initialRoute: '',
        initialRouteRedirect: false
      }
    default:
      return state;
  }
};