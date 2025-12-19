import { serve } from "bun";
import index from "@/index.html";
import type { Message } from "@/lib/types";

let messages: Message[] = [];

const server = serve({
  routes: {
    // Serve index.html for all unmatched routes.
    "/*": index,

    "/clear": {
      async POST(req) {
        messages = [];
        console.log("Cleared log messages");

        // publish to all websocket subscribers
        server.publish("log-updates", JSON.stringify({ type: "clear" }));

        return Response.json({ status: 200, message: "Cleared" });
      },
    },
    "/log": {
      async GET(req) {
        return Response.json(messages);
      },
      async POST(req) {
        const body = await req.json();
        messages.push(body);
        console.log("Log from client:", body);

        // publish to all websocket subscribers
        server.publish("log-updates", JSON.stringify(body));

        return Response.json({ status: 200, message: "Logged" });
      },
    },
  },

  fetch(req, server) {
    // upgrade the request to a WebSocket
    if (server.upgrade(req)) {
      return; // do not return a Response
    }

    return new Response("Upgrade to websocket failed", { status: 500 });
  },

  websocket: {
    open(ws) {
      console.debug("WebSocket connection opened");

      // subscribe to a topic
      ws.subscribe("log-updates");

      // send all messages
      if (messages.length > 0) {
        messages.map((msg) => ws.send(JSON.stringify(msg)));
      }
    },
    message(ws, message) {
      console.debug("Received message:", message);

      // Broadcast the message to all subscribers of the "log-updates" topic
      ws.publish("log-updates", message);
    },
    close(ws) {
      console.debug("WebSocket connection closed");
      ws.unsubscribe("log-updates");
    },
  },

  development: process.env.NODE_ENV !== "production" && {
    // Enable browser hot reloading in development
    hmr: true,

    // Echo console logs from the browser to the server
    console: true,
  },
});

console.log(`🚀 Server running at ${server.url}`);
