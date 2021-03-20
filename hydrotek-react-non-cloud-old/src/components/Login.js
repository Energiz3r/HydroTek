import React from 'react'
import { connect } from 'react-redux'
import animatedComponent from '../utils/animatedComponent'

class Login extends React.Component {
  constructor(props) {
    super(props)
    this.state = {
      username:'',
      password:'',
      displayMessage: window.displayMessage
    }
  }
  onUsernameChange = (e) => {
    this.setState({...this.state, username: e.target.value})
  }
  onPasswordChange = (e) => {
    this.setState({...this.state, password: e.target.value})
  }
  render() {
    return (
      <div className='main-menu-container'>
        <h1>Login</h1>
        <p>{this.state.displayMessage}</p>
        <form action="index.php" method="post" className='login-form'>
          <input type="text" name="username" placeholder='username' onChange={this.onUsernameChange} />
          <input type="password" name="password" placeholder="password" onChange={this.onPasswordChange} />
          <input type="submit" class='button-base button-base-input button-selected' />
        </form>
      </div>
    )
  }
}

export default animatedComponent(Login)