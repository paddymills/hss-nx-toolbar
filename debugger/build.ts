import tailwind from "bun-plugin-tailwind";

await Bun.build({
  plugins: [tailwind],
  entrypoints: ["./src/server.ts"],
  outdir: "./dist",
  target: "bun",
});
