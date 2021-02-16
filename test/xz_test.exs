defmodule XZTest do
  use ExUnit.Case, async: true

  test("successfully compresses and decompresses small data buffer") do
    data = "Hello World"
    {:ok, compressed} = XZ.compress(data)
    {:ok, ^data}      = XZ.decompress(compressed)
  end

  test "successfully compresses and decompresses large data buffer" do
    data = :crypto.strong_rand_bytes(10240)
    {:ok, compressed} = XZ.compress(data)
    {:ok, ^data}      = XZ.decompress(compressed)
  end
end
