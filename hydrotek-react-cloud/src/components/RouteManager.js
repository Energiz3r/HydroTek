import { connect } from 'react-redux'
import { setInitialRoute, setRoute } from '../actions/UIActions'
import {
  buildForDev,
  serverAPILocation,
  serverLocation,
  devDefaultRoute,
  devDummyLogin
} from '../config'
import {
  setUserDetails,
  setLoginStatusApp,
  setLoginStatusFacebook,
  setLoginStatus
} from '../actions/loginActions'

class RouteManager extends React.Component {
    constructor(props) {
      super(props)
      this.state = {
        
      }
    }
    componentDidUpdate = (prevProps) => {
      const prev = prevProps.UI.route
      const next = this.props.UI.route
      if (prev != next) {
        console.log("Route was: " + prev + ", setting to: " + next)
        this.props.history.push(serverLocation + next)
      }
    }
    componentDidMount = () => {
      console.log("Route manager loading.")

      const url = this.props.location.pathname //get the current url
      console.log("Route manager started at: " + url)
      this.props.dispatch(setInitialRoute(url))

      if (buildForDev) {
        if (devDummyLogin) {
          this.props.dispatch(setUserDetails({email: 'tim.eastwood@hotmail.com', name: 'Tim Devwood'}))
          this.props.dispatch(setLoginStatusApp(true))
          this.props.dispatch(setLoginStatusFacebook(true))
          setTimeout(()=>{
            this.props.dispatch(setLoginStatus(true)) // delay the login so the login modal can fade out
            this.props.dispatch(setRoute(devDefaultRoute))
          }, 1000)
        } else {
          this.props.dispatch(setRoute('login')) //show as not logged in
        }
      }

      else if (!this.props.login.loggedIn) {
        this.props.dispatch(setRoute('login')) //if not logged in
      }

      //the below checks if we were found to be logged in on the initial load of the app and fixes the state. shouldn't happen.
      // else {
      //   fetch(serverAPIEndpoint, {
      //     method: 'POST',
      //     cache: 'no-cache',
      //     headers: {
      //       'Content-Type': 'application/json'
      //     },
      //     body: JSON.stringify({
      //       "action": "checkLoginSimple"
      //     })
      //   })
      //     .then(result => result.json())
      //     .then(
      //       (result) => {
      //         if (result.loggedIn === "1") {
      //           console.log(result)
      //           this.props.dispatch(setUserDetails(result))
      //           this.props.dispatch(setLoginStatusApp(true))
      //           this.props.dispatch(setLoginStatusFacebook(true))
      //           setTimeout(()=>{
      //             this.props.dispatch(setLoginStatus(true)) // delay the login so the login modal can fade out
      //             this.props.dispatch(setRoute('home'))
      //           }, 1000)
      //         } else {
      //           console.log("Failed simple login check:")
      //           console.log(result)
      //         }
      //       },
      //       (error) => {
      //         console.log("Error:")
      //         console.log(error)
      //       }
      //     )
      //}

    }
    render() {
      return null
    }
  }
  
  export default connect(state=>state)(RouteManager)