export interface ServerInfo {
  guildId: number;
  channelId: number;
  displayName: string;
  description: string;
  address: string;
  identifier: string,
  rconPassword: string;
  exposePublicIp: 0|1;
  publicIpPort: number;
}