
mysql = require('mysql')
globals = require('./globals')

const initialCreate = false // deletes (if exists) and re-creates the entire database. inserts default users
const dataReset = false // purges all device data points

const dbServer = "localhost"
const dbName = "hydrotek"
const dbUser = "hydro"
const dbPw = "hydro123"

/*
  database: hydrotek
  user: hydro
  password: hydro123
*/

let config = {
  host: dbServer
  ,user: dbUser
  ,password: dbPw
  ,multipleStatements: true
  ,database: dbName
  ,typeCast: function castField( field, useDefaultTypeCasting ) {
    // We only want to cast bit fields that have a single-bit in them. If the field
    // has more than one bit, then we cannot assume it is supposed to be a Boolean.
    if ((field.type === "BIT") && (field.length === 1)) {
      var bytes = field.buffer();
      // A Buffer in Node represents a collection of 8-bit unsigned integers.
      // Therefore, our single "bit field" comes back as the bits '0000 0001',
      // which is equivalent to the number 1.
      return(bytes[0] === 1)
    }
    return(useDefaultTypeCasting())
  }
}

//const db = mysql.createConnection(config)
var db = mysql.createPool(config)

const showErr = (err) => {
  console.log('╟ SQL Error: ' + err.code + ', ' + err.sqlMessage)
}

initialiseDatabase = () => {

  console.log("╔═══════════════════════════════╗")
  console.log("║  Database (re)build started!  ║")
  console.log("╚═══════════════════════════════╝")

  config.database = null
  db.query("DROP DATABASE " + dbName, (err, result, fields) => {
    if (err) {
      console.log("╟ DROP error:")
      showErr(err)
    } else {
      console.log("╟ Database deleted"); 
    }

    let tempdb = mysql.createConnection(config)

    tempdb.query("CREATE DATABASE IF NOT EXISTS " + dbName, (err, result) => {
      if (err) {
        console.log("╟ Create DB error:")
        showErr(err)
        tempdb.end()
      } else {
        console.log("╟ Database created")
        tempdb.end()

        //re-create the pool
        config.database = dbName
        db = mysql.createPool(config)
        db.query("USE " + dbName, (err) => {
          if (err) {
            console.log("╟ error with query: 'USE " + dbName + "'")
            showErr(err)
          } else {

            // create users table
            db.query("CREATE TABLE IF NOT EXISTS users (\
              userId int NOT NULL AUTO_INCREMENT,\
              nick varchar(255) NULL,\
              isAdmin bit NOT NULL DEFAULT 0,\
              lastSeen datetime NULL,\
              isGlobalBanned bit NOT NULL DEFAULT 0,\
              email varchar(1024) NULL,\
              password varchar(1024) NULL,\
              PRIMARY KEY (userId))", function (err) {
              if (err) { showErr(err) } else { console.log("╟ 'users' table created") }
    
              db.query("INSERT INTO users (nick, isAdmin, email, password) VALUES ('Energizer', 1, 'tim.eastwood@hotmail.com', SHA2(?, 256))",['jiblet123'],  (err, result) => {
                if (err) { showErr(err) } else { console.log("╟ default user created") }
              })
    
            })
  
          }
        })
      }      
    })

    console.log("╟ DB (re)creation complete.")
    console.log("╚════════════════════════════════")

  })
}

//basic test to make sure at least one of the tables exists
if (!initialCreate) {
  db.query("SELECT 1 FROM users LIMIT 1", (err) => {
    if (err) {
      console.log("Error connecting to database - stopping startup of Hydrotek:")
      showErr(err)
    } else {

      globals.databaseConnected = true // signals any other process waiting for valid DB connection to go ahead
      console.log("(database) Connected!")

    }
  })
} else {
  initialiseDatabase()
};

module.exports = db; 
