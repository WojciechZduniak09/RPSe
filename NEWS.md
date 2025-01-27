# RPSe development news #
Copyright (C) 2024, 2025 Wojciech Zduniak <githubinquiries.ladder140@passinbox.com>, Marcin Zduniak

## v0.4.0-alpha.4 ##
1. Moved broadcast and recevier loops to *rpsecore-discovery*.
2. Made *broadcast_data* free after use in *rpsecore-gamemode1.c*.
3. Added delay to broadcaster to make the message always be visible regardless of device.
4. Fixed *_rpse_broadcast_getBroadcastAddress()*.
5. Changed receiver and broadcaster ports to *port 70000*.
6. Made broadcast verifier static and added it to the receiver for safety.
7. Added *DRD testing* to *debug.sh*.
8. Added mutexes to interval waiter for thread safety.
9. Fixed *rpse_broadcast_receiveBroadcast()*.
10. Added safer deletion in duplicate DLL deletion function.
11. Made broadcaster and receiver ports reusable.
12. Added longer delays in interval waiter to improve system resource usage.
13. Fixed static function naming conventions in moveDef (I think that's the file),
14. Added more efficient and detailed error messages and no longer using *rpsecore-error.h* but *perror()*.
15. Made broadcaster broadcast once.
16. Fixed broadcast regex macros.
17. Imposed naming restrictions for PvP string parsing safety.
18. Made broadcaster in discovery source broadcast 3 times at random intevals to ensure messages don't conflict.
19. Made broadcast receiver ignore broadcasts from the host system.
20. Made DLL position deleter not create indirect memory leaks.
21. Fixed regex constants.
22. Fixed duplicate DLL node deleter.

# Older NEWS information #
Older news information can be found in the ```ONEWS.md``` archive file.

