/**
 * Copyright Mirage MC (C) 2023.
 * The mirage server logger functions.
 * 
 * @author TBCM - TBCMDev
 */


function concatenate(x, ...strs) {
    const ret = x + strs[0];

    for(var i = 1; i < strs.length; i++) ret += '\n' + (' ' * x.length) + strs[i];

    return ret;
}

/**
 * Logs an info message to the console.
 */
module.exports.

info = 
(...msgs) => console.log(concatenate("[INFO] ", msgs));

/**
 * Logs an error message to the console.
 */
module.exports.

error = 
(...msgs) => console.log(concatenate("[ERROR] ", msgs));

/**
 * Logs a warning message to the console.
 */
module.exports.

warning = 
(...msgs) => console.log(concatenate("[WARNING] ", msgs));

/**
 * Logs a fatal message to the console.
 */
module.exports.

fatal = 
(...msgs) => console.log(concatenate("[FATAL] ", msgs));

/**
 * Logs a severe message to the console.
 */
module.exports.

severe = 
(...msgs) => console.log(concatenate("[SEVERE] ", msgs));