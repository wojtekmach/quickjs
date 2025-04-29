defmodule QuickJS.MixProject do
  use Mix.Project

  def project do
    [
      app: :quickjs,
      version: "0.1.0",
      elixir: "~> 1.16",
      compilers: [:elixir_make] ++ Mix.compilers(),
      make_env: fn ->
        %{
          "FINE_INCLUDE_DIR" => Fine.include_dir(),
          "QUICKJS_INCLUDE_DIR" => "#{__DIR__}/vendor/quickjs/build/include",
          "QUICKJS_LIB_DIR" => "#{__DIR__}/vendor/quickjs/build/lib"
        }
      end,
      start_permanent: Mix.env() == :prod,
      deps: deps()
    ]
  end

  def application do
    [
      extra_applications: [:logger]
    ]
  end

  defp deps do
    [
      {:elixir_make, "~> 0.9.0"},
      {:fine, "~> 0.1.0"}
    ]
  end
end
