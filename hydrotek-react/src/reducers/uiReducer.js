const reducerDefaultState = {
  appName: 'UI',
  loggedIn: false,
  location: 'home',
  lastLocation: ''
}

export default (state = reducerDefaultState, action) => {
  switch (action.type) {
    case 'SET_LOCATION':
      return {
        ...state,
        location: action.location
      }
    case 'SET_LAST_LOCATION':
      return {
        ...state,
        lastLocation: action.location
      }
    case 'SAVE_REPORT':
      return{
        ...state,
        reports: [
          ...state.reports,
          action.report
        ]
      }
    default:
      return state
  }
};