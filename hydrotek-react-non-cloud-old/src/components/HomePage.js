import React from 'react'
import animatedComponent from '../utils/animatedComponent'
import {setLocation} from '../actions/uiActions'
import {connect} from 'react-redux'

class Home extends React.Component {
  constructor(props) {
    super(props)
    this.state = {
      userName: window.username,
      displayMessage: window.displayMessage
    }
  }
  render() {
    return (
      <div className='main-menu-container'>
        <h2>Menu</h2>
        <p>{this.state.displayMessage == "" ? ("Hello, " + this.state.userName + ".") : this.state.displayMessage}</p>
          <div className="button-container">
          <p><button className='button-base button-selected' onClick={() => { this.props.dispatch(setLocation('Charts')) }} >Charts</button></p>
          </div>
      </div>
    )
  }
}

export default animatedComponent(connect(state=>state)(Home))