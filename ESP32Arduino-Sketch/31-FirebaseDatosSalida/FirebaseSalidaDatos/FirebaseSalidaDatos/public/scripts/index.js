const loginElement = document.querySelector('#login-form');
const contentElement = document.querySelector("#content-sign-in");
const userDetailsElement = document.querySelector('#user-details');
const authBarElement = document.querySelector("#authentication-bar");

// Elements for GPIO states
const stateElement = document.getElementById("state");


// Button Elements
const btnOn = document.getElementById('btnOn');
const btnOff = document.getElementById('btnOff');

// Database path for GPIO states
var dbPathOutput = 'tarjeta/led/digital/2';

// Database references
var dbRefOutput = firebase.database().ref().child(dbPathOutput);

// MANAGE LOGIN/LOGOUT UI
const setupUI = (user) => {
  if (user) {
    //toggle UI elements
    loginElement.style.display = 'none';
    contentElement.style.display = 'block';
    authBarElement.style.display ='block';
    userDetailsElement.style.display ='block';
    userDetailsElement.innerHTML = user.email;

    //Update states depending on the database value
    dbRefOutput.on('value', snap => {
        if(snap.val()==1) {
            stateElement.innerText="ON";
        }
        else{
            stateElement.innerText="OFF";
        }
    });

    // Update database uppon button click
    btnOn.onclick = () =>{
        dbRefOutput.set(1);
    }
    btnOff.onclick = () =>{
        dbRefOutput.set(0);
    }

  // if user is logged out
  } else{
    // toggle UI elements
    loginElement.style.display = 'block';
    authBarElement.style.display ='none';
    userDetailsElement.style.display ='none';
    contentElement.style.display = 'none';
  }
}