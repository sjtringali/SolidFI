// Copyright (c) 2026 Scott Tringali. All rights reserved.

import { describe, it } from 'node:test';
import assert from 'node:assert/strict';

import { Provider } from './Provider';
import { Converter } from './Converter';
import { Chain } from './Chain';
import { Parameters } from './Parameters';

class UpperCase extends Provider<string, string> {
    convert(value: string, params: Parameters): string {
        return value.toUpperCase();
    }
}

class NonEmptyFilter extends Provider<string, string | null> {
    accepts(value: string): boolean {
        return value.length > 0;
    }

    convert(value: string, params: Parameters): string {
        return value;
    }
}

class PrefixedConverter extends Provider<string, string> {
    constructor(private readonly prefix: string) {
        super();
    }

    convert(value: string, params: Parameters): string {
        return `${this.prefix}${value}`;
    }
}

describe('Provider', () => {
    it('resolve() delegates to convert() in the concrete subclass', () => {
        const p = new UpperCase();
        assert.equal(p.resolve('hello', {}), 'HELLO');
    });

    it('prepare Transform runs before convert()', () => {
        const p = new UpperCase();
        p.prepare = { apply: (v: string) => v.trim() };
        assert.equal(p.resolve('  hello  ', {}), 'HELLO');
    });

    it('finalize Transform runs after convert()', () => {
        const p = new UpperCase();
        p.finalize = { apply: (v: string) => `${v}!` };
        assert.equal(p.resolve('hello', {}), 'HELLO!');
    });

    it('prepare and finalize both run when set', () => {
        const p = new UpperCase();
        p.prepare  = { apply: (v: string) => v.trim() };
        p.finalize = { apply: (v: string) => `${v}!` };
        assert.equal(p.resolve('  hello  ', {}), 'HELLO!');
    });

    it('is assignable to Converter', () => {
        const p: Converter<string, string> = new UpperCase();
        assert.equal(p.resolve('world', {}), 'WORLD');
    });

    it('optional accepts() defined on a subclass is present and callable', () => {
        const p = new NonEmptyFilter();
        assert.equal(p.accepts?.('hello'), true);
        assert.equal(p.accepts?.(''), false);
    });

    it('optional accepts() is absent when not defined on the subclass', () => {
        const p: Converter<string, string> = new UpperCase();
        assert.equal(p.accepts, undefined);
    });

    it('can be installed in a Chain and participates in normal chain resolution', () => {
        const chain = new Chain<string, string | null>();
        chain.install(1, 'non-empty', new NonEmptyFilter());
        assert.equal(chain.resolve('hello', {}), 'hello');
        assert.equal(chain.resolve('', {}), null);
    });

    it('constructor arguments work as expected in subclasses', () => {
        const p = new PrefixedConverter('Mr. ');
        assert.equal(p.resolve('Smith', {}), 'Mr. Smith');
    });
});
