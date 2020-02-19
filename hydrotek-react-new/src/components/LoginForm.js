import { connect } from 'react-redux'
import { FadeTransform } from 'react-animation-components'
import { webpackDevServer, serverAPILocation } from '../config'
import { setRoute } from '../actions/UIActions'
import { setLoginStatus, setLoginStatusApp, setUserDetails } from '../actions/loginActions'

class LoginForm extends React.Component {
  constructor(props) {
    super(props)
    this.state = {
      termsAccepted: false,
      awaitingCreateResponse: false,
      email
    }
  }
  onAcceptTerms = () => {
    console.log("terms accept")
    this.setState({
      ...this.state,
      termsAccepted: !this.state.termsAccepted
    })
  }
  onCreateAccount = () => {
    console.log("Creating login...")
    this.setState({
      ...this.state,
      awaitingCreateResponse: true
    })
    fetch(serverAPILocation, {
      method: 'POST',
      cache: 'no-cache',
      headers: {
        'Content-Type': 'application/json'
      },
      body: JSON.stringify({
        "action": "createAppUser"
      })
    })
      .then(result => result.json())
      .then(
        (result) => {
          if (result.loggedIn === "1") {
            console.log(result)
            this.props.dispatch(setUserDetails(result))
            this.props.dispatch(setLoginStatusApp(true))
            setTimeout(()=>{
              this.props.dispatch(setLoginStatus(true)) // delay the login so the login modal can fade out
              this.props.dispatch(setRoute('/home'))
            }, 1000)
          } else {
            console.log("Failed create account:")
            console.log(result)
          }
        },
        (error) => {
          console.log("Error:")
          console.log(error)
        }
      )
  }
  render() {
    const {
      loggedInFacebook,
      loggedInApp,
      createAccount,
      responseErrorMessage,
      email,
      name
    } = this.props.login
    const {
      awaitingCreateResponse,
      termsAccepted
    } = this.state
    return (

      <div className={"modal-wrapper" + ((loggedInFacebook && loggedInApp) ? ' modal-fade-out' : "")}>
        <FadeTransform in transformProps={{enterTransform: 'translateY(1.5rem)', exitTransform: 'translateY(-1.5rem)'}}>
          <div className="modal-outer-container">
            <div className="modal-inner-container">

              <div className="login-form-container">

                <h2>Welcome to App!</h2>

                {loggedInFacebook && !loggedInApp && !createAccount && <p>Logging in...</p>}

                {(!loggedInFacebook && !webpackDevServer) && 
                  <div
                    className="fb-login-button"
                    data-width=""
                    data-size="large"
                    data-button-type="continue_with"
                    data-auto-logout-link="false"
                    data-use-continue-as="true"
                    >
                  </div>}

                {createAccount && <div className="login-create-container">
                  <p>Please accept the terms and click Confirm to finish setting up your account</p>
                  <label className='login-create-label'>Name:</label>
                  <input type="text" className='login-create-textinput' value={name} disabled></input>
                  <label className='login-create-label'>Email:</label>
                  <input type="email" className='login-create-textinput' value={email} placeholder="eg: user@mail.com" disabled={email == ''}></input>
                  <div className="terms-container">
                    <p className={"terms-paragraph"}>
                      Accept <a href='/terms.html'>terms and conditions</a>?
                    </p>
                    <label className="checkbox-container">
                      <input
                        type="checkbox"
                        checked={termsAccepted ? "checked" : ''}
                        onChange={this.onAcceptTerms}
                      />
                      <span className="checkbox-checkmark"></span>
                    </label>
                  </div>
                  <button onClick={this.onCreateAccount} className='button-default' disabled={!termsAccepted || awaitingCreateResponse || email == ''}>
                    {awaitingCreateResponse ? <span className="fa fa-spinner fa-spin"></span> : "Confirm"}
                  </button>
                </div>}

                {responseErrorMessage != '' && <p className='login-error-text'>{responseErrorMessage}</p>}

                {!loggedInFacebook && <p className='modal-large-text'>Please log in using Facebook to continue.</p> }
                {!loggedInFacebook && <p className='modal-disclaimer-text'>We do not store any of your personal information except your email address, which is used only to alert you about your devices if you turn alerts on. We will not share your information with anyone or use it for any other purpose except with your express permission.</p> }

              </div>

            </div>
          </div>
        </FadeTransform>
      </div>
    )
  }
}

export default connect(state=>state)(LoginForm)