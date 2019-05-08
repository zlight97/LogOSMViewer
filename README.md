# LogOSMViewer
Notes from before
Load log file data to view timestamped data
can be played back at realtime. played forwards, paused, backwards at various speeds

Interface must be flexable (functions of a class probably) will change later

GET urls from postman confirmed working
For choords:
http://nominatim.openstreetmap.org/reverse?lat=37.78073&lon=-122.41898

For node IDS:
https://api.openstreetmap.org/api/0.6/way/108389188

Download file at this url:
https://overpass-api.de/api/map?bbox=-122.5872,37.6773,-122.2521,37.8307


need to look into overpass, probably the best way to go about it


[timeout:10][out:json];(node(around:22.5,37.77944,-122.42526);way(around:22.5,37.77944,-122.42526););out tags geom(37.777634750327046,-122.43199467658997,37.78284129296774,-122.41783261299133);relation(around:22.5,37.77944,-122.42526);out geom(37.777634750327046,-122.43199467658997,37.78284129296774,-122.41783261299133);

above somehow gives nearby features (probably)

while this one gives encompasing featrures?

[timeout:10][out:json];is_in(37.77944,-122.42526)->.a;way(pivot.a);out tags bb;out ids geom(37.777634750327046,-122.43199467658997,37.78284129296774,-122.41783261299133);relation(pivot.a);out tags bb;

these give results as json which is better? maybe. List is too large though, need to figure out how it's sorted or a way to sort it