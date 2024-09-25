/**
 * Copyright Mirage MC (C) 2023.
 * 
 * A file containing authentication functions for validating a users identity on the web.
 * 
 * Notes:
 *  use jwt?
 * 
 * 
 * @author TBCM - TBCMDev
 */

/**
 * The key name of the authentication object in a request object.
 */
module.exports.AUTH_HEADER_NAME = "auth";


/**
 * The route name that should listen for new requests to establish a token secured
 * connection. IE the login route.
 */
module.exports.NEW_AUTH_ROUTE = "/login"; 

/**
 * the amount of time in ms that a users token is alive for.
 * 
 * **20 minutes or 1200 seconds in miliseconds.**
 * 
 */
module.exports.TOKEN_ALIVE_TIME = 1200 * 1000; // 20 minutes or 1200 seconds in miliseconds.

/**
 * Authenticates a user by parsing the request provided.
 * 
 * @param {Request} request The request object from express.

 * @returns true if the user is authenticated, false otherwise.
 */
module.exports.

hauth =
(request) => {
    const auth = request.headers.auth;

    if(!vparams(auth)) return false;

    const [username_or_email, password] = [auth.username_or_email, auth.password];

    if(!vparams(username_or_email, password)) return false;
}

/**
 * Creates a new authentication profile and key for the user coming from the request
 * parameter. 
 * 
 * @see TOKEN_ALIVE_TIME - The time the token is valid for. 
 * 
 * If something goes wrong, null is returned.
 * @param {Request} request The request object from express.
 * 
 * @returns A non-prepared response object to give the user their token.
 */
module.exports.

newauth =
(request) => {
    const auth = request.headers.auth;

    if(!vparams(auth)) return null;

    const [username_or_email, password] = [auth.username_or_email, auth.password];

    if(!vparams(username_or_email, password)) return null;

    return request; // todo
}