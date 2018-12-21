# Cpprestsdk-Server-for-twiiterAPI
* at any time, a client web browser can be run. The client enter a search term, and it will issue a query
 * against the Twitter API.  It will process the result set as follows:
 Produce a histogram from the top 100 results, matching number of tweets to dates.  Save this in a MongoDB collection.  The query term should be the document name.
 * - Note: query terms that are user handles (@name) or hashtags (#tag) will
 *   naturally show user activity or topic trends
 * - Note: authentication for the Twitter API uses OAuth.
 *
 * The server program do three things.
 *   - It serves an index.html file in response to GET /
 *   - It serves other files in response to GET /files/<filename>
 *   - It serves JSON, pulled from MongoDB, in response to GET /data/term
 *
 * The HTML5 component allows the user to enter a search term.  In response to that term, the page will issue an AJAX
 * request to the server to get some JSON data, and then it should use that JSON
 * data to initialize a chart. 
 *
 * the program use Microsoft's cpprestsdk. 
 The server uses Mongodb and twitcurl (so the first step is to install both Mongodb and twitcurl on ubuntu)
 
 Then, cd build_dir, cmake .. and then make 
