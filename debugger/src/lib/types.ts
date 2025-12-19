export interface Message {
  timestamp: string;
  partFile?: string;
  level: "info" | "warn" | "error" | "debug";
  message: string;
  filename?: string;
  lineno?: number;
  [key: string]: any;
}
