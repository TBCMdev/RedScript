/**
 * Copyright Mirage MC (C) 2023.
 * 
 * A js file that carries all boilerplate code from ../server.js.
 * 
 * @author TBCM - TBCMDev
 */


/*
 * An object wrapper for an error recieved from the mirage server.
 */
class ServerSideError{

    /**
     * All known Error Codes Recieved from the Mirage server in a JS object.
     */
    static ErrorCodes = 
    {
        OTHER: -2,
        SUCCESS: -1,
        INVALID_AUTH: 0,
        
        // todo
    }
    static getErrorReason(code){
        for(const x of Object.entries(this.ErrorCodes)){
            if(x[1] == code) return x[0];
        }
        return null;
    }
    code;reason;

    /**
     * Constructs a new ServerSideError instance.
     * If reason is not provided, ServerSideError.ErrorCodes is used to determine 
     * the error reason.
     * @param {String} reason The error reason.
     * @param {Number} code An integer error code. @see ServerSideError.ErrorCodes
     */
    constructor(code = -1, reason = null){
        this.code = code;
        this.reason = reason == null ? 
            ServerSideError.getErrorReason(code) : 
            reason;
    }
};

/**
 * An object enum wrapper for a server response.
 * Static fields are used here.
 */
class ServerResponse {
    /**
     * Used when a users auth key is outdated or is invalid.
     */
    static authFail = 
    {
        reason: "Authentication Failure.",
        code: ServerSideError.ErrorCodes.INVALID_AUTH
    }
};

/**
 * The port the mirage server listens on.
 */
module.exports.SERVER_LISTEN_PORT = 5000;

/**
 * Prepares a variable (res) of any type to be sent back to a client.
 * Note that your intented response will be wrapped in the body of the response,
 * res.body . 
 * @returns the sendable response.
 */
module.exports.prepareResponsePayload =
    (res) => ({ ok: !(res instanceof ServerSideError) , body: res});



module.exports.ServerResponse = ServerResponse;
module.exports.ServerSideError = ServerSideError;

