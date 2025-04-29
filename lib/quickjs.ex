defmodule QuickJS do
  @on_load :__on_load__

  def __on_load__ do
    path = :filename.join(:code.priv_dir(:quickjs), ~c"libquickjs_nif")

    case :erlang.load_nif(path, 0) do
      :ok -> :ok
      {:error, reason} -> raise "failed to load NIF library, reason: #{inspect(reason)}"
    end
  end

  def add1(_a) do
    :erlang.nif_error("nif not loaded")
  end

  def eval(_code) do
    :erlang.nif_error("nif not loaded")
  end

  def eval2(_code, _binding) do
    :erlang.nif_error("nif not loaded")
  end

  defmacro sigil_JS({:<<>>, _, [binary]}, []) do
    {:<<>>, meta, parts} = Code.string_to_quoted!("\"#{binary}\"")

    {code, vars} =
      Enum.reduce(parts, {"", []}, fn
        binary, {code, vars} when is_binary(binary) ->
          {code <> binary, vars}

        {:"::", _,
         [{{:., _, [Kernel, :to_string]}, _, [{var_name, _, context} = var]}, {:binary, _, nil}]},
        {code, vars}
        when is_atom(var_name) and is_atom(context) ->
          {code <> "#{var_name}", [{var_name, var}] ++ vars}
      end)

    code = {:<<>>, meta, [code]}

    quote do
      QuickJS.eval2(unquote(code), Map.new(unquote(vars), fn {k, v} -> {to_string(k), v} end))
    end
  end
end
