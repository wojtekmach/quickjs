defmodule QuickJSTest do
  use ExUnit.Case, async: true

  test "add/1" do
    assert QuickJS.add1(2) == 3
  end

  test "eval/1" do
    assert QuickJS.eval("1 + 2") == "3"
  end

  test "sigil" do
    import QuickJS

    x = 2
    assert ~JS"#{x} + 1" == "3"
  end
end
