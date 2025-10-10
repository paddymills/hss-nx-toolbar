export interface Message {
  timestamp: string;
  partFile?: string;
  level: "info" | "warn" | "error" | "debug";
  message: string;
  [key: string]: any;
}
