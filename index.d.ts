declare module "@larseble/farsight" {
  export async function connectToLeague(): Promise<boolean>
  export function disconnectFromLeague(): boolean
  export function makeSnapshot(): any
  export function setOffsets(): boolean
  export function isReady(): boolean
  export function setChampionNames(): any
  export let autoImportChampions: boolean
  export let autoImportOffsets: boolean
}