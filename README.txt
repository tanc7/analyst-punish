# Analyst Punishing Framework

Inspired by "Suicide Linux". 

I actually got this idea over two days ago after reading some chapters of Surreptious Software from the University of Arizona. Instead of just exiting when being analyzed or tampered with, why not retaliate? 

# Realistically speaking... 

It's more than likely a decent analyst has a specific VM just for that reason that they can just restore a snapshot of. All this really does is buy more time. But it's a great opportunity to keep defenders guessing

Furthermore the obfuscation game is constantly changing. No way can you keep payloads fully undetectable. I'll write some proof of concepts, but it's up to everyone who forked it to keep obfuscating it. Clearly having a Class named Antidebug would be detectable. Duh.


To add

1. Software tamperproofing and watermarking as punish-triggers
-> You check the integrity or hash of the watermark, kind of like a StackGuard Canary, but at the START of the program, and not when StackGuard has been triggered from a buffer overflow condition
-> Check return addresses of payloads (some may forget to return a specific integer or boolean expression, causing punishes
2. Full unit tests against both remnux and flare vm
3. Full suite of debugger detections for Linux, Windows, MacOS

