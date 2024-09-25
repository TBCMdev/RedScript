# -------------------------------------------------
# COPYRIGHT TBCMDev 2024, All Rights Reserved.
# This content may be distributed or published in the given condition that credit is not taken.
# modification or tampering with any file in these directories is permitted, yet will almost certainly
# lead to undefined behaviour in game.
# ------------------------------------------------- 

# Gets first char
$data modify storage redscript:temp char set string storage redscript:temp $(p1).data 0 1

# Converts char to ascii repr integer. We don't use default registers here for simplicity.
$scoreboard player set _EXTREG_CONVERT_ _extreg  