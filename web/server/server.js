/**
 * Copyright Mirage MC (C) 2023.
 * 
 * The Mirage Web server, providing data to all website instances using express js.
 * Authentication methods are housed here, along with all web endpoints.
 * 
 * @author TBCM - TBCMDev
 * 
 * 
 * @note This file is in experimental development stages and may have flaws. Use the github error system to notify me of any errors so I can fix them.
 * 
 */

// Other files made:
const { hauth, newauth, AUTH_HEADER_NAME, NEW_AUTH_ROUTE }                           = require("./util/auth");
const { ServerResponse, ServerSideError, SERVER_LISTEN_PORT, prepareResponsePayload} = require("./util/sutil");
const { info, error, severe, fatal, warning}                                         = require("./util/logging");
const { routes }                                                                     = require("./util/routes");


// database connection:
const db = require("./util/db");


const express = require("express");
const cors = require("cors");
const bodyParser = require("body-parser");

// boilerplate
const _app = express();
_app.use(cors());
_app.use(bodyParser.json());

// first contact for all routes
_app.all("*", (req, res, next) => {
    // Has user logged in?
    if(!hauth(req)) {
        // they havent
        if(req.route.path == NEW_AUTH_ROUTE) 
        {
            // get user credentials, validate, and create new session for user.
            const auth = newauth(req);

            // returns new auth key if user credentials are valid, 
            // otherwise an error prompting the user to login again.
            return res.json(auth == null ? ServerResponse.authFail : prepareResponsePayload(auth));
        }

        // tells the console that the guest user is unauthorized,
        // and passes false to the next() functions authed parameter, indicating
        // the custom route defined that the user is un-authed. Some pages such as
        // the profile page or account page will reject access instantly if this field is false.
        warning("Un-authed user with auth header of: ", req.headers[AUTH_HEADER_NAME]);
        return res.json(prepareResponsePayload( next(req, res, false ) ));
    }
    // they have
    info("User logged in: ", req.ip);
    res.status(200); // can be changed in next() function

    /**
     * -----------------
     * -   READ HERE   -
     * -----------------
     * 
     * All custom routes created in this file must return their json response.
     * The returned value can be anything, even null. It is good practice to be an object.
     * 
     * The next function from your route also must have a parameter specifying if the user is authenticated or not:
     * 
     * (req, res, authed)
     * 
     * If your return value is an error, examples like on line 25 show how to properly use them.
     * 
     * @see ServerResponse houses static constants used for different responses. Includes errors.
     * @see ServerSideError An object wrapper for an error recieved from the server.
     * 
     * @see prepareResponsePayload prepareResponsePayload will prepare whatever is returned from next() to be sent back. Please read its contents.
     * 
     */
    return res.json(prepareResponsePayload( next(req, res, true ) ));
})

// init routes
for(const route of Object.values(routes)){
    try{
        switch(route.method){
            case "GET": {
                _app.get(route.method, route.func);
                break;
            }
            case "POST": {
                _app.get(route.method, route.func);
                break;
            }
            case "DELETE": {
                _app.delete(route.method, route.func);
                break;
            }
            case "ALL": {
                _app.all(route.method, route.func);
                break;
            }
            case "PUT": {
                _app.put(route.method, route.func);
                break;
            }
            default: {
                _app.get(route.method, route.func);
                break;
            }
        }
        info("Registered route '" + route.route + "'. ");
    }catch(e){
        fatal("Route could not be established: name: " + route.route)
    }
}

_app.listen(SERVER_LISTEN_PORT, () => {
    info("Server Listening on " + SERVER_LISTEN_PORT + ".");
})


