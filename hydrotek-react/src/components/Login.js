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
      <div>
        <h1>Login</h1>
        <p>Please log in</p>
        <p>{this.state.displayMessage}</p>
        <form action="index.php" method="post">
          <input type="text" name="username" placeholder='username' onChange={this.onUsernameChange} />
          <input type="password" name="password" placeholder="password" onChange={this.onPasswordChange} />
          <input type='hidden' name="token" value="skjdfhslkg78h34flkjeho84f7y0894rj2oui43jhnf8923hr09fh24oi57h28437fh" />
          <input type="submit" />
        </form>
        <a href="#" onClick={this.props.setLoggedIn}>Force Login</a>
      </div>
    )
  }
}

export default animatedComponent(Login)