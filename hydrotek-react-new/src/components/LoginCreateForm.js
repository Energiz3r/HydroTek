import React from 'react';
import { connect } from 'react-redux';
import { socket } from '../utils/handlers/client';
import { nickMinLength, nickMaxLength } from '../config.js';

class loginCreateForm extends React.Component {
  constructor(props) {
    super(props);
    this.state={
      email: '',
      emailIsValid: false,
      nick: '',
      nickIsValid: false,
      password1: '',
      password2: '',
      passwordsAreValid: false,
      termsAccepted: false,
      waitingForCreateResponse: false,
      createResponse: ''
    }
  }
  handleCreateResponse = (response) => { //listener action
    if (response == "success") {
      this.props.goToLoginTab();
    } else {
      this.setState({
        ...this.state,
        waitingForCreateResponse: false,
        createResponse: response
      })
    }
  }
  componentDidMount = () => {
    //socket.on('login create response', this.handleCreateResponse); //create listener
  }
  componentWillUnmount = () => {
    //socket.removeListener('login create response', this.handleCreateResponse); //destroy listener
  }
  onFormSubmit=(e)=>{
    e.preventDefault();
    if (
      //check all the form info first
      this.state.emailIsValid &&
      this.state.termsAccepted &&
      !this.state.waitingForCreateResponse &&
      this.state.nick.length > nickMinLength &&
      this.state.nick.length < nickMaxLength &&
      this.state.password1.length > 3 &&
      this.state.password1 == this.state.password2
      ) {
      this.setState({
        ...this.state,
        waitingForLoginResponse: true,
        loginResponse: ''
      })

      socket.emit('request create user', {
        email: this.state.email,
        nick: this.state.nick,
        password: this.state.password1
      });

    }
    //TODO do some more advanced form validation at this point
    else {
      this.setState({
        ...this.state,
        waitingForCreateResponse: false,
        createResponse: 'please complete all form elements'
      })
    }
  }
  onPassword1Change=(e)=>{
    const password1 = e.target.value;
    this.setState({
      ...this.state,
      password1
    })
  }
  onPassword2Change=(e)=>{
    const password2 = e.target.value;
    this.setState({
      ...this.state,
      password2
    })
  }
  onEmailChange=(e)=>{
    const email = e.target.value;
    //check if email is valid
    if (!email || email.match( /^(([^<>()[\]\\.,;:\s@\"]+(\.[^<>()[\]\\.,;:\s@\"]+)*)|(\".+\"))@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\])|(([a-zA-Z\-0-9]+\.)+[a-zA-Z]{2,}))$/ )) {
      this.setState({
        ...this.state,
        email,
        emailIsValid: true
      })
    } else {
      this.setState({
        ...this.state,
        email,
        emailIsValid: false
      })
    }
  }
  onNickChange=(e)=>{
    const nick = e.target.value;
    //check if email is valid
    if (!nick || (nick.length < nickMaxLength && nick.length > nickMinLength)) {
      this.setState({
        ...this.state,
        nick,
        nickIsValid: true
      })
    } else {
      this.setState({
        ...this.state,
        nick,
        nickIsValid: false
      })
    }
  }
  render() {
    return (

      <div className="loginFormContainer">
        <form className="loginForm" onSubmit={this.onFormSubmit}>

          {(this.state.email != '' && !this.state.emailIsValid) &&
            <p className="loginMessage">Please enter a valid email</p>}
          {this.state.createResponse != '' ? <p className="nickSetFailedReason">{this.state.createResponse}</p> : ''}

          <input 
            className={"loginInput guestNickInput"}
            type='text'
            placeholder="email"
            onChange={this.onEmailChange}
            value={this.state.email || ''}
            spellCheck="false"
          />

          <input 
            className={"loginInput guestNickInput"}
            type='text'
            placeholder="nick"
            onChange={this.onNickChange}
            value={this.state.nick || ''}
            spellCheck="false"
          />

          {(this.state.password1 != this.state.password2 && !this.state.password1 == '') &&
            <p className="loginMessage">Both passwords must match!</p>}

          <input
            className={"loginInput guestNickInput"}
            type='password'
            placeholder="password"
            onChange={this.onPassword1Change}
            value={this.state.password1 || ''}
          />

          <input
            className={"loginInput guestNickInput"}
            type='password'
            placeholder="password"
            onChange={this.onPassword2Change}
            value={this.state.password2 || ''}
          />

          <div className="terms-container terms-container-create">
            <p className={"terms-paragraph"}>Accept <a href='terms.html' >terms and conditions</a>?</p>
            <label className="checkbox-container">
              <input
                type="checkbox"
                checked={this.state.termsAccepted ? "checked" : ''}
                onChange={() => {
                  if (this.state.termsAccepted) { this.setState({
                    ...this.state,
                    termsAccepted: false
                  }) }
                  else { this.setState({
                    ...this.state,
                    termsAccepted: true
                  }) }
                }}
              />
              <span className="checkbox-checkmark"></span>
            </label>
            
          </div>

          <p className="loginMessage">
            <button
              className='loginButton'
              //check for conditions that would cause the button to be disabled
              disabled={!this.state.emailIsValid ||
                !this.state.nick.length > 3 ||
                !(this.state.password1 == this.state.password2) ||
                !this.state.termsAccepted ||
                this.state.email.length < 5 ||
                this.state.waitingForCreateResponse
              }
            >
              {!this.state.waitingForCreateResponse ? "Register" : "Registering... "}
              {this.state.waitingForCreateResponse && <span className="fa fa-spinner fa-spin"></span>}
            </button>
          </p>

          <p className="loginMessage"><i>Your email address will be encrypted and is used only for you to log in. We will never email you, ask you for your password, or share your information with anyone.</i></p>

        </form>
      </div>
    );
  };
}

export default connect(state=>state)(loginCreateForm);