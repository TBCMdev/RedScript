/**
 * Copyright Mirage MC (C) 2023.
 * 
 * All routes used by the mirage server.
 * @author TBCM - TBCMDev
 */


/**
 * An object encapsulator class that holds the route and function for when the route is
 * accessed.
 */
class Route {
    /**
     * @type {string}
     */
    method;
    /**
     * @type {string}
     */
    route;
    /**
     * @type {function(Request, Response, boolean>):any}
    */
    func;

    /**
     * Creates a new route.
     * @param {string} method
     * @param {string} route
     * @param {function(Request, Response, boolean):any} func
     */
    constructor(method, route, func) {
        this.method = method;
        this.route = route;
        this.func = func;
    }
}

/**
 * @type {Object.<string, Route>}
 */
module.exports.routes = {}

/**
 * Route for pinging the server. Mostly for testing.
 */
module.exports.routes.
    ping = new Route("GET","/ping", (req, res, authed) => null); // all we are looking for is a ping, so no payload body is provided.

/* ADD ROUTES HERE
// using syntax: 
// module.exports.routes.<your_route> = new Route(<http_method>, <route>, (req, res, authed) => {
    // must return something to be sent back as response body.
 })*/